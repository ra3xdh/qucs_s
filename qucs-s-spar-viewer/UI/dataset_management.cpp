/*
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "qucs-s-spar-viewer.h"

///
/// @brief Remove all loaded files from the application
///
/// Performs complete cleanup:
/// 1. Collects all file IDs from remove buttons
/// 2. Calls removeFile() for each file individually
/// 3. Removes all file paths from the file system watcher
/// 4. Clears the watchedFilePaths map
///
/// @note This also removes all associated traces and markers
/// @note File watcher paths are cleared to stop monitoring
void Qucs_S_SPAR_Viewer::removeAllFiles() {
  // Remove files
  QStringList fileIDs;
  for (int i = 0; i < List_RemoveButton.size(); i++) {
    fileIDs.append(List_RemoveButton.at(i)->objectName());
  }

  // Remove each file by ID
  for (const QString &ID : fileIDs) {
    removeFile(ID);
  }

  // Remove all paths from the file watcher
  if (!fileWatcher->files().isEmpty()) {
    fileWatcher->removePaths(fileWatcher->files());
  }

  // Clear the watchedFilePaths map
  watchedFilePaths.clear();
}

///
/// @brief Remove a specific file by its ID
///
/// Removes a file and all associated data:
/// 1. Locates the file widgets by button ID
/// 2. Deletes the file name label and remove button
/// 3. Collapses the layout to remove gaps
/// 4. Removes all traces belonging to the dataset
/// 5. Removes the dataset from the data structure
/// 6. Updates the dataset combo box
///
/// @param ID Unique identifier of the file to remove
///
/// @note The ID corresponds to the button's objectName
/// @note All traces associated with this dataset are automatically removed
/// @note The datasets combo box is updated
void Qucs_S_SPAR_Viewer::removeFile(QString ID) {
  // Find the row number of the button to remove
  int row_to_remove = -1;
  QString dataset_to_remove;

  for (int i = 0; i < List_RemoveButton.size(); i++) {
    if (List_RemoveButton.at(i)->objectName() == ID) {
      row_to_remove = i;
      dataset_to_remove = List_FileNames.at(i)->text();
      delete List_RemoveButton.takeAt(i); // Use takeAt() instead of at()
      delete List_FileNames.takeAt(i);    // Removes AND returns the pointer
      break;
    }
  }

  // Once removed the widgets, fill the gap in the layout
  if (row_to_remove >= 0) {
    removeAndCollapseRow(FilesGrid, row_to_remove);

    // Find all traces belonging to this dataset and remove them
    removeTracesByDataset(dataset_to_remove);
  }

  datasets.remove(ID);
  removeTracesByDataset(ID);

  // Update datasets' combobox
  int index = QCombobox_datasets->findText(ID);
  QCombobox_datasets->removeItem(index);
  updateTracesCombo();
}

///
/// @brief Remove file via UI
///
/// Called when a remove button is clicked. Identifies which file
/// to remove based on the sender's object name and calls removeFile(QString).
///
/// @note This is a Qt slot connected to remove button click signals
/// @note Gets the button ID from sender()->objectName()
/// @see removeFile(QString
/// )
void Qucs_S_SPAR_Viewer::removeFile() {
  QToolButton *button = qobject_cast<QToolButton *>(sender());
  QString ID = button->objectName();
  removeFile(ID);
}

///
/// @brief Add a single file using QFileInfo
///
/// Wrapper function for adding a single file. Used when opening
/// files from command line.
///
/// @param fileInfo QFileInfo object containing file path information
///
/// @note Validates file existence before attempting to load
/// @note Shows warning dialog if file doesn't exist
/// @note Internally calls addFiles(QStringList) with single-item list
void Qucs_S_SPAR_Viewer::addFile(const QFileInfo &fileInfo) {
  if (fileInfo.exists()) {
    QStringList fileNames;
    fileNames.append(fileInfo.absoluteFilePath());
    addFiles(fileNames);
  } else {
    QMessageBox::warning(this, tr("Error"),
                         tr("The file or directory does not exist."));
  }
}

///
/// @brief Open file dialog to add files
///
/// Displays a file selection dialog allowing the user to choose files.
/// Supported formats:
/// - Touchstone files (.s1p, .s2p, .s3p, .s4p)
/// - Qucsator data files (.dat)
/// - NGspice data files (.ngspice.dat)
///
/// @note Multiple file selection is enabled
/// @note Dialog starts in user's home directory
/// @see addFiles(QStringList)
///
void Qucs_S_SPAR_Viewer::addFile() {
  QFileDialog dialog(this, QStringLiteral("Select S-parameter data files"),
                     QDir::homePath(),
                     tr("S-Parameter Files (*.s1p *.s2p *.s3p *.s4p);;"
                        "Data Files (*.dat *.ngspice.dat);;"
                        "All Files (*.*)"));
  dialog.setFileMode(QFileDialog::ExistingFiles);

  QStringList fileNames;
  if (dialog.exec()) {
    fileNames = dialog.selectedFiles();
  }

  addFiles(fileNames);
}

///
/// @brief Add multiple files to the dataset
///
/// Main file loading function that:
/// 1. Checks for duplicate files already in dataset
/// 2. Determines file format from extension
/// 3. Parses files using appropriate reader:
///    - Touchstone format (.snp): readTouchstoneFile()
///    - Qucsator format (.dat): readQucsatorDataset()
///    - NGspice format (.ngspice): readNGspiceData()
/// 4. Validates parsed data for S-parameters
/// 5. Creates UI widgets for valid files
/// 6. Adds datasets to internal data structure
/// 7. Registers files with file system watcher
/// 8. Updates dataset combo box
/// 9. Adds optional derived traces
/// 10. Applies default visualizations
///
/// @param fileNames List of full file paths to add
///
void Qucs_S_SPAR_Viewer::addFiles(QStringList fileNames) {
  int existing_files =
      this->datasets.size(); // Get the number of entries in the map
  QString filename;

  if (existing_files == 0) {
    // Reset limits
    this->f_max = -1;
    this->f_min = 1e30;
  }

  // Remove from the list of files those that already exist in the database
  QStringList files_dataset = datasets.keys();

  for (int i = 0; i < fileNames.length(); i++) {
    filename = QFileInfo(fileNames.at(i)).fileName();
    // Check if this file already exists
    QString new_filename = filename.left(filename.lastIndexOf('.'));

    if (new_filename.endsWith(".dat")) {
      // This file has extension .dat.ngspice
      new_filename = new_filename.left(new_filename.length() - 4);
    }

    if (files_dataset.contains(new_filename)) {
      // Remove it from the list of new files to load
      fileNames.removeAt(i);

      // Pop up a warning
      QMessageBox::information(this, tr("Warning"),
                               tr("This file is already in the dataset."));
    }
  }

  // Read files
  int widget_counter = existing_files;
  int n_files = fileNames.length(); // Number of files to be added
  QStringList files_filtered; // Some of the files included may be discarded for
                              // not having s-parameter data. This list contain
                              // only the files to be added
  for (int i = existing_files; i < existing_files + n_files; i++) {
    // Create the file name label
    QString filename = QFileInfo(fileNames.at(i - existing_files)).fileName();

    // Determine the file extension
    QString fileExtension =
        QFileInfo(fileNames.at(i - existing_files)).suffix().toLower();

    QMap<QString, QList<double>> file_data;

    // Use appropriate function based on the file extension
    if (fileExtension.startsWith("s") && fileExtension.endsWith("p")) {
      file_data = readTouchstoneFile(fileNames.at(i - existing_files));
    } else if (fileExtension == "dat") {
      file_data = readQucsatorDataset(fileNames.at(i - existing_files));
    } else if (fileExtension == "ngspice") {
      file_data = readNGspiceData(fileNames.at(i - existing_files));
    } else {
      qWarning() << "Unsupported file extension: " << fileExtension;
      continue; // Skip unsupported files
    }

    if (file_data.isEmpty()) {
      // Stop the load process and remove file from the list of files to be
      // added
      continue;
    } else {
      // It must contain basic S-parameter data
      if (file_data["n_ports"].at(0) == 0) {
        continue;
      }
    }
    files_filtered.append(filename);

    // Create widgets at this point. It's necessary to ensure that the files to
    // be loaded contain S-parameter data
    CreateFileWidgets(filename, widget_counter);
    widget_counter++;

    // Add data to the dataset
    QString dataset_name =
        filename.left(filename.lastIndexOf('.')); // Remove file extension
    if (fileExtension == "ngspice") {
      // These files have extension .dat.ngspice. Remove the extension again to
      // have only the file name
      dataset_name = dataset_name.left(dataset_name.length() - 4);
    }

    datasets[dataset_name] = file_data;

    // Add file to watchedFilePaths map
    watchedFilePaths[dataset_name] = fileNames.at(i - existing_files);

    // Add new dataset to the trace selection combobox
    QCombobox_datasets->addItem(dataset_name);

    // Add optional traces based on number of ports
    addOptionalTraces(file_data);

    // Update traces
    updateTracesCombo();
  }

  // Apply default visualizations based on file types
  applyDefaultVisualizations(files_filtered);

  // Set up file watcher for the newly added files
  setupFileWatcher();
}
