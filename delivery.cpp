#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

double conditions(double a, double c, double g) {
    if (c == 0) {
        cout << "Ошибка: c не может быть равно нулю (деление на ноль).\n";
        return NAN;
    }
    if (a + g < 0) {
        cout << "Ошибка: подкорневое выражение не может быть меньше нуля.\n";
        return NAN;
    }
    double znamenatel = 12.75 + g * g;
    if (znamenatel == 0) {
        cout << "Ошибка: знаменатель не может быть равен нулю.\n";
        return NAN;
    }
    return (sin(a / c) + sqrt(a + g)) / znamenatel;
}

void firstProgram() {
    double a, c, g;
    cout << "Введите a, c, g: ";
    cin >> a >> c >> g;
    double result = conditions(a, c, g);
    if (!isnan(result)) {
        cout << "y == " << result << "\n";
    }
}

void secondProgram() {
    int n;
    cout << "Введите размер массива n: ";
    cin >> n;
    if (n <= 0) {
        cout << "Ошибка: размер массива должен быть больше нуля.\n";
        return;
    }
    vector<int> a(n), b(n);
    cout << "Введите элементы массива a: ";
    for (int &x : a) cin >> x;
    for (int i = 0, sum = 0; i < n; i++) {
        sum += a[i];
        b[i] = sum / (i + 1);
    }
    cout << "Массив b: ";
    for (int x : b) cout << x << " ";
    cout << endl;
}

void thirdProgram() {
    int a, b, c, d;
    cout << "Введите a, b, c, d: ";
    cin >> a >> b >> c >> d;
    if (a != b && a != c && a != d)
        cout << "1";
    else if (b != a && b != c && b != d)
        cout << "2";
    else if (c != a && c != b && c != d)
        cout << "3";
    else
        cout << "4";
    cout << endl;
}

void fourthProgram() {
    int sum = 0, num;
    for (int i = 1; i <= 3; i++) {
        cout << "Введите " << i << "-е число: ";
        cin >> num;
        sum += num;
    }
    cout << "Сумма трёх чисел равна: " << sum << endl;
}

void fifthProgram() {
    int time;
    cout << "Введите время в минутах: ";
    cin >> time;
    if (time < 0) {
        cout << "Ошибка: время не может быть отрицательным.\n";
        return;
    }
    cout << time << " мин - это " << time / 60 << " часов " << time % 60 << " минут" << endl;
}

void printInfo() {
    cout << "\nМеню:" << endl;
    cout << "1. Посчитать значение сложной тригонометрической функции" << endl;
    cout << "2. Получить массив" << endl;
    cout << "3. Задача с условием чисел" << endl;
    cout << "4. Сумма трёх чисел" << endl;
    cout << "5. Пересчёт величины временного интервала" << endl;
    cout << "6. Выйти из программы" << endl;
}

int main() {
    while (true) {
        printInfo();
        int choice;
        cout << "Введите номер программы: ";
        cin >> choice;
        switch (choice) {
            case 1: firstProgram(); break;
            case 2: secondProgram(); break;
            case 3: thirdProgram(); break;
            case 4: fourthProgram(); break;
            case 5: fifthProgram(); break;
            case 6: return 0;
            default: cout << "Неверный выбор!" << endl;
        }
    }
}
