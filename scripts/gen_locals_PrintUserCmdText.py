import os
import re
import json
import datetime
import chardet

SOURCE_DIR = "../src"       # Папка с cpp/h файлами
now = datetime.datetime.now()
res = int(now.timestamp() * 1000)
JSON_FILE = f"localization_{res}.json"
LOG_FILE = f"localization_log_{res}.txt"

key_counter = 1
localization = {}
log_lines = []

# Регулярка для поиска PrintUserCmdText(iClientID, L"...")
pattern = re.compile(
    r'PrintUserCmdText\s*\(\s*[^,]+,\s*L"(.*?)"(.*?)\)', 
    re.DOTALL
)

def generate_key():
    global key_counter
    key = f"MSG_{key_counter:04}"
    key_counter += 1
    return key

for root, _, files in os.walk(SOURCE_DIR):
    for file in files:
        if file.endswith((".cpp", ".h")):
            path = os.path.join(root, file)
            
            # Определяем кодировку через chardet
            try:
                with open(path, "rb") as f:
                    raw_data = f.read()
                    detected = chardet.detect(raw_data)
                    encoding = detected['encoding'] if detected['encoding'] else 'utf-8'
                content = raw_data.decode(encoding)
            except Exception as e:
                log_lines.append(f"[ERROR] Не удалось прочитать файл {path}: {e}")
                continue

            matches = pattern.findall(content)
            if not matches:
                continue

            log_lines.append(f"Файл: {path} - найдено {len(matches)} сообщений")

            for literal, extra in matches:
                key = generate_key()
                # Добавляем в словарь локализации
                localization[key] = {
                    "ru": literal,
                    "en": ""
                }

                # Составляем замену с сохранением параметров
                # extra может быть: , param1, param2 ...
                replacement = f'GetLocalized("{key}"){extra}'
                # Заменяем L"..." на GetLocalized
                old_text = f'L"{literal}"{extra}'
                content = content.replace(old_text, replacement)
                log_lines.append(f"  -> {literal} => {key}")

            # Перезаписываем файл с заменами
            with open(path, "w", encoding="utf-8", newline='') as f:
                f.write(content)

# Сохраняем JSON
with open(JSON_FILE, "w", encoding="utf-8") as f:
    json.dump(localization, f, ensure_ascii=False, indent=4)

# Сохраняем лог
with open(LOG_FILE, "w", encoding="utf-8") as f:
    f.write("\n".join(log_lines))

print(f"Обработано {key_counter-1} сообщений, JSON сохранён в {JSON_FILE}, лог в {LOG_FILE}")
