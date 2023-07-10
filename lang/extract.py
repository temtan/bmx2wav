import re

def extract_strings(input_file):
    with open(input_file, 'r', encoding='cp932') as f:
        content = f.readlines()
        # This regular expression pattern finds strings inside double quotes
        pattern = r'^\s*RSID_STR.*?"([^"]*)"'
        with open('strings.txt', 'w', encoding='cp932') as f_out:
            for line in content:
                matches = re.findall(pattern, line)
                for match in matches:
                    f_out.write(match + '\n')

extract_strings('japanese.rc')
