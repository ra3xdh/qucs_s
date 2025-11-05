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

void Qucs_S_SPAR_Viewer::removeAllFiles() {
  // Remove files
  QStringList fileIDs;
  for (int i = 0; i < List_RemoveButton.size(); i++) {
    fileIDs.append(List_RemoveButton.at(i)->objectName());
  }

  // Remove each file by ID
  for (const QString& ID : fileIDs) {
    removeFile(ID);
  }

  // Remove all paths from the file watcher
  if (!fileWatcher->files().isEmpty()) {
    fileWatcher->removePaths(fileWatcher->files());
  }

  // Clear the watchedFilePaths map
  watchedFilePaths.clear();
}

void Qucs_S_SPAR_Viewer::removeFile(QString ID) {
  // Find the row number of the button to remove
  int row_to_remove = -1;
  QString dataset_to_remove;

  for (int i = 0; i < List_RemoveButton.size(); i++) {
    if (List_RemoveButton.at(i)->objectName() == ID) {
      row_to_remove     = i;
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

// This function is called whenever a s-par file is intended to be removed from
// the map of datasets
void Qucs_S_SPAR_Viewer::removeFile() {
  QToolButton* button = qobject_cast<QToolButton*>(sender());
  QString ID          = button->objectName();
  removeFile(ID);
}

// Wrapper of void "Qucs_S_SPAR_Viewer::addFiles(QStringList fileNames)". It is
// needed to open a Touchstone file from command line
void Qucs_S_SPAR_Viewer::addFile(const QFileInfo& fileInfo) {
  if (fileInfo.exists()) {
    QStringList fileNames;
    fileNames.append(fileInfo.absoluteFilePath());
    addFiles(fileNames);
  } else {
    QMessageBox::warning(this, tr("Error"),
                         tr("The file or directory does not exist."));
  }
}

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
  int n_files        = fileNames.length(); // Number of files to be added
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
