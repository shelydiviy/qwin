#!/bin/bash

# Проверка количества аргументов
if [ $# -ne 1 ]; then
    echo "Использование: $0 {start|stop}"
    exit 1
fi

ACTION=$1
CONFIG_PATH="/home/sprut/qwin/config/config.json"
BINARY_PATH="/home/sprut/qwin/build/tiny-csgo-mirror-system"
LOG_FILE="/home/sprut/qwin/logs/system.log"

# Проверка существования бинарного файла
if [ ! -x "$BINARY_PATH" ]; then
    echo "Ошибка: Бинарный файл '$BINARY_PATH' не найден или не имеет прав на выполнение."
    exit 1
fi

# Проверка существования конфигурационного файла
if [ ! -f "$CONFIG_PATH" ]; then
    echo "Ошибка: Конфигурационный файл '$CONFIG_PATH' не найден."
    exit 1
fi

if [ "$ACTION" == "start" ]; then
    echo "Запуск системы..."
    nohup "$BINARY_PATH" --config "$CONFIG_PATH" > "$LOG_FILE" 2>&1 &
    sleep 2 # Даем время на запуск
    if pgrep -f "$(basename "$BINARY_PATH")" > /dev/null; then
        echo "Система успешно запущена. Логи доступны в $LOG_FILE"
    else
        echo "Ошибка: Не удалось запустить систему. Проверьте логи: $LOG_FILE"
        exit 1
    fi
elif [ "$ACTION" == "stop" ]; then
    echo "Остановка системы..."
    if pgrep -f "$(basename "$BINARY_PATH")" > /dev/null; then
        pkill -f "$(basename "$BINARY_PATH")"
        sleep 2 # Даем время на завершение
        if pgrep -f "$(basename "$BINARY_PATH")" > /dev/null; then
            echo "Ошибка: Не удалось остановить систему."
            exit 1
        else
            echo "Система остановлена."
        fi
    else
        echo "Система уже не запущена."
    fi
else
    echo "Использование: $0 {start|stop}"
    exit 1
fi
