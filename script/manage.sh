#!/bin/bash

# Переход в корневую директорию проекта
SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
cd "$SCRIPT_DIR/.."

# Путь к бинарному файлу
BINARY="./build/tiny-csgo-mirror-system"

# Создание директории для логов, если её нет
LOG_DIR="./logs"
if [ ! -d "$LOG_DIR" ]; then
    mkdir -p "$LOG_DIR"
fi

# Увеличение лимита открытых файлов
ulimit -n 65535

# Функция запуска
start() {
    echo "Starting the system..."
    if pgrep -f "$BINARY" > /dev/null; then
        echo "System is already running!"
        exit 1
    fi

    # Запуск программы с явным указанием пути к конфигурационному файлу
    nohup "$BINARY" --config "$(pwd)/config/config.json" > "$LOG_DIR/system.log" 2>&1 &
    echo "System started successfully."
}

# Функция остановки
stop() {
    echo "Stopping the system..."
    if ! pgrep -f "$BINARY" > /dev/null; then
        echo "System is not running!"
        exit 1
    fi
    pkill -f "$BINARY"
    echo "System stopped successfully."
}

# Функция перезапуска
restart() {
    stop
    sleep 2
    start
}

# Обработка аргументов
case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    restart)
        restart
        ;;
    *)
        echo "Usage: $0 {start|stop|restart}"
        exit 1
esac