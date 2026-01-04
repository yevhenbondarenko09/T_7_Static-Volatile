#include <iostream>
#include <thread>   // Для створення окремого потоку (імітація заліза)
#include <chrono>
#include <windows.h>
using namespace std; // Використання простору імен std

/**
 * 1. VOLATILE: Ця змінна може бути змінена "апаратною частиною".
 * Без volatile компілятор може вирішити, що в циклі while(hw_flag == 0)
 * значення не змінюється, і зробить цей цикл нескінченним (оптимізація).
 */
volatile int hw_flag = 0;
volatile int sensor_data = 0;

/**
 * 2. STATIC FILE SCOPE: Ця функція доступна ТІЛЬКИ в цьому файлі.
 * Це гарний тон для "внутрішніх" функцій драйвера.
 */
static void internal_driver_log(const char* msg) {
    cout << "[DRIVER INTERNAL] " << msg << endl;
}

static void generate_report(int total_events) {
    cout << "\n========== ПІДСУМКОВИЙ ЗВІТ ==========" << endl;
    cout << "Статус системи: Завершено" << endl;
    cout << "Всього подій опрацьовано: " << total_events << endl;
    cout << "Останнє значення сенсора: " << sensor_data << endl;
    cout << "======================================" << endl;
}
/**
 * Функція, що імітує роботу заліза або переривання (ISR)
 * Вона працює в іншому потоці паралельно з main.
 */
void hardware_process(int data) {
    static int func_counter = 0;
    func_counter++;
    internal_driver_log("Залізо ініціалізується...");
    this_thread::sleep_for(chrono::seconds(2));
    cout << "Лічильник: " << func_counter << endl;
    internal_driver_log("Подія сталася! Змінюю дані...");
    sensor_data = data;  // Записуємо дані в регістр
    hw_flag = 1;       // Піднімаємо прапорець готовності
        }


int main() {
    SetConsoleOutputCP(1251);
    cout << "Очікування сигналу від заліза..." << endl;
   
    // Запускаємо "залізо" в окремому потоці
    int value_to_pass = 8;
    thread hw_thread(hardware_process, value_to_pass);

    /**
     * ГОЛОВНИЙ ЦИКЛ ОЧІКУВАННЯ (Polling)
     * Якщо hw_flag НЕ volatile, компілятор при оптимізації може перетворити це на:
     * if (hw_flag == 0) while(true); // бо він не бачить, щоб хтось у main змінював hw_flag
     */
    while (hw_flag == 0) {
        // Просто чекаємо. В реальних системах тут може бути Low Power Mode.
    }

    cout << "\n[MAIN] Сигнал отримано!" << endl;
    cout << "[MAIN] Дані з сенсора: " << sensor_data << endl;
    hw_thread.join();
    sensor_data = 0;
    hw_flag = 0;
    value_to_pass = 16;
    thread hw2_thread(hardware_process, value_to_pass);
    while (hw_flag == 0) {
        // Просто чекаємо. В реальних системах тут може бути Low Power Mode.
    }
    cout << "\n[MAIN] Сигнал отримано!" << endl;
    cout << "[MAIN] Дані з сенсора: " << sensor_data << endl;



    // Чекаємо завершення потоку заліза перед виходом
    hw2_thread.join();
    generate_report(2);
    return 0;
}

