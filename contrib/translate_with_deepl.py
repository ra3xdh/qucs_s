import os
import xml.etree.ElementTree as ET
import deepl  # Import DeepL API

# Set up DeepL API key
token = 'your_deepL_api_key'  # Replace with your DeepL API key
translator = deepl.Translator(token)

# select target lang
def translate_text(text, source_lang="EN", target_lang="XX"):
    """
    Translates text using DeepL API.
    """
    try:
        # Call DeepL API for translation
        result = translator.translate_text(text, source_lang=source_lang, target_lang=target_lang)
        return result.text
    except Exception as e:
        print(f"Error translating text: {e}")
        return text  # Return the original text in case of an error

def process_ts_file(input_file, output_file):
    """
    Processes a .ts XML file to translate `unfinished` entries.
    """
    tree = ET.parse(input_file)
    root = tree.getroot()

    for context in root.findall("context"):
        for message in context.findall("message"):
            source = message.find("source")
            translation = message.find("translation")
            if translation is not None and translation.get("type") == "unfinished":
                if source is not None:
                    source_text = source.text
                    # Translate the source text
                    translated_text = translate_text(source_text)
                    print(f"Translating: {source_text} -> {translated_text}")
                    translation.text = translated_text
                    translation.attrib.pop("type", None)  # Remove 'unfinished' attribute

    # Save the updated file
    tree.write(output_file, encoding="utf-8", xml_declaration=True)

# Example usage
input_ts_file = "qucs_xx.ts"  # Replace with your input file path
output_ts_file = "qucs_xx_translated.ts"  # Replace with your desired output file path
process_ts_file(input_ts_file, output_ts_file)
