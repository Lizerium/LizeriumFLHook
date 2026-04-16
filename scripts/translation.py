import requests
import json
import re

INPUT_FILE = "localization_1756534561082.json"
OUTPUT_FILE = "output.json"
LOG_FILE = "translation_placeholders.log"
TRANSLATE_URL = "http://192.168.64.128:5001/translate"

# Регулярка для поиска плейсхолдеров типа %s, %0.0f, %d и т.п.
PLACEHOLDER_RE = re.compile(r"%(\d*\.\d*)?[sdf]")

with open(INPUT_FILE, "r", encoding="utf-8") as f:
    data = json.load(f)

log_lines = []
total_keys = len(data)
translated_count = 0

for idx, (key, value) in enumerate(data.items(), 1):
    ru_text = value.get("ru", "")
    if not ru_text:
        continue

    if PLACEHOLDER_RE.search(ru_text):
        # Логируем строки с плейсхолдерами и не переводим
        log_lines.append(f"{key}: {ru_text}")
        print(f"[SKIP] {key} содержит плейсхолдеры, пропущено ({idx}/{total_keys})")

    try:
        response = requests.post(
            TRANSLATE_URL,
            data={
                "q": ru_text,
                "source": "ru",
                "target": "en",
                "format": "text"
            }
        )
        translated_text = response.json().get("translatedText", "")
        value["en"] = translated_text
        translated_count += 1
        print(f"[OK] {key} переведено: {translated_text} ({idx}/{total_keys})")
    except Exception as e:
        log_lines.append(f"{key}: Ошибка перевода: {e}")
        print(f"[ERROR] {key}: {e} ({idx}/{total_keys})")

print(f"\nПереведено {translated_count} из {total_keys} сообщений.")

# Сохраняем переведённый JSON
with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
    json.dump(data, f, ensure_ascii=False, indent=4)

# Сохраняем лог с плейсхолдерами
with open(LOG_FILE, "w", encoding="utf-8") as f:
    f.write("\n".join(log_lines))

print(f"JSON сохранён в {OUTPUT_FILE}, лог плейсхолдеров: {LOG_FILE}")
