import re

def replace_strings(input_file, translated_file):
    # Load translations
    with open(translated_file, 'r', encoding='cp932') as f:
        translations = [line.strip() for line in f]

    with open(input_file, 'r', encoding='cp932') as f:
        content = f.readlines()

    new_content = []
    idx = 0
    pattern = r'"([^"]*)"'
    for line in content:
        if line.strip().startswith("RSID_STR"):
            match = re.search(pattern, line)
            if match:
                line = line.replace(match.group(1), translations[idx])
                idx += 1
        new_content.append(line)

    # Write new content to a new rc file
    with open('translated.rc', 'w', encoding='cp932') as f_out:
        f_out.write(''.join(new_content))

replace_strings('japanese.rc', 'translated.txt')
