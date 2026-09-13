#include <iostream>
#include <iomanip>
#include <string>  
#include <sstream>
#include <cstring>
#include <cmath>

using namespace std;


// Цвета текста
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"


// Флаги
bool output_simple = false;
bool output_rpn = false;


//Прототипы функций.
void menu_commands(int choice);


// Структуры и классы
struct Node {
    string value;   
    Node* next;
    Node* prev;

    Node(string data): value(data), next(nullptr), prev(nullptr) {}
};

class DoublyLinkedList {
    private:
        Node* head = nullptr;

        void print_function(const Node* head) {
            if (head == nullptr) {
                return;
            }
            print_function(head->next);
            cout << head->value << " ";
        }

    public:
        DoublyLinkedList() = default;

        void push_begin(string data) { // добавление в начало
            Node* new_node = new Node(data);
            new_node->next = head;
            if (head != nullptr) {
                head->prev = new_node;
            }
            head = new_node;
        }

        void push_end(string data) { // добавление в конец
            if (head == nullptr) {
                push_begin(data);
                return;
            }
            Node* new_node = new Node(data);
            Node* last_node = get_last_node();
            new_node->prev = last_node;
            last_node->next = new_node;
        }

        Node* get_last_node() { // получение последнего элемента
            if (head == nullptr) {
                return nullptr;
            }

            Node* temp_head = head;
            while (temp_head->next) {
                temp_head = temp_head->next;
            }
            return temp_head;
        }

        int size() { // получение размера списка
            int count = 0;
            Node* temp_head = head;
            while (temp_head) {
                count++;
                temp_head = temp_head->next;
            }
            return count;
        }

        string pop() { // вытащить элемент из стека
            Node* temp = nullptr;
            string val;
            if (head == nullptr) {
                cout << RED << "Ошибка: стек пуст!" << RESET << endl;
                return "";
            }
            val = head->value;
            temp = head;
            head = head->next;
            if (head != nullptr) {
                head->prev = nullptr;
            }
            delete temp;
            return val;
        }

        bool is_empty() { // проверка на пустой список
            return head == nullptr;
        }

        string peek() { // посмотреть элемент стека
            if (head == nullptr) return "";
            return head->value;
        }

        void print() { // вывод списка
            print_function(head);
        }

        void clear() { // удаление списка
            if (!head) {
                return;
            }
            while (head) {
                head->value = "";
                Node* temp = head;
                head = head->next;
                delete temp;
            }
        }

        ~DoublyLinkedList() {
            clear();
        }
};

class DynamicArray {
private:
    string* data = nullptr;
    int array_size = 0;
    int capacity = 0;

    void resize() {
        int new_capacity = (capacity == 0) ? 1 : capacity + 1;
        string* new_data = new string[new_capacity];

        for (int i = 0; i < array_size; i++) {
            new_data[i] = data[i];
        }

        delete[] data;
        data = new_data;
        capacity = new_capacity;
    }

public:
    DynamicArray() = default;

    void push_back(string value) {
        if (array_size >= capacity) {
            resize();
        }
        data[array_size++] = value;
    }

    string get(int index) {
        if (index < 0 || index >= array_size) {
            return "";
        }
        return data[index];
    }

    int size() {
        return array_size;
    }

    string output() {
        string res = "";
        for (int i = 0; i < array_size; i++) {
            res += data[i] + " ";
        }
        return res;
    }

    void clear() {
        delete[] data;
        data = nullptr;
        array_size = 0;
        capacity = 0;
    }

    ~DynamicArray() {
        clear();
    }
};


// объявление стеков и массивов
DoublyLinkedList stack_op;   // стек для операторов
DoublyLinkedList stack_calc; // стек для вычисления
DynamicArray polish_array;

// Объявление переменных
string input_str;            // изначальное выражение
//string polish_str;           // выражение в польской нотации
double final_result;         // результат выражения


// Служебные функции
void separation() {
    cout << "\n";
}

void stream_clear() {
    cin.clear();
    cin.ignore(10000, '\n');
}

void waiting() {
    separation();
    separation();
    cout << RED << "Нажмите Enter для продолжения..." << RESET << endl;
    cin.get();
}

void stack_steps_waiting() {
    separation();
    cin.get();
}

void error_detention() {
    stream_clear();
    separation();
    separation();
    cout << RED << "Нажмите Enter для продолжения..." << RESET << endl;
    cin.get();
}

void clear_screen() {
    system("cls");
}

void delete_all_data() {
    stack_op.clear();
    stack_calc.clear();
    input_str = "";
    polish_array.clear();
    final_result = 0;
}


// Вспомогательные функции для алгоритма ОПЗ
bool is_operand(string token) {
    if (token.empty()) return false;

    for (int i = 0; i < token.length(); i++) {
        if (!isdigit(token[i])) {
            return false;
        }
    }

    return true;
}

bool is_operation(string token) {
    if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
        return true;
    }
    else {
        return false;
    }
}

bool is_func(string token) {
    return (token == "sin" || token == "cos");
}

bool is_valid_simple(string input_str) {
    if (input_str.empty()) return false;

    DoublyLinkedList brackets_stack;
    stringstream stream(input_str);
    string token;
    string last_token = "";
    int token_count = 0;

    while (stream >> token) {
        token_count++;

        if (token == "(") {
            if (last_token != "" && !is_operation(last_token) && last_token != "(" && !is_func(last_token)) {
                brackets_stack.clear();
                return false;
            }
            brackets_stack.push_begin("(");
        }
        else if (token == ")") {
            if (brackets_stack.is_empty()) return false;
            brackets_stack.pop();
            if (last_token == "(" || is_operation(last_token) || is_func(last_token)) {
                brackets_stack.clear();
                return false;
            }
        }
        else if (is_func(token)) {
            if (last_token != "" && !is_operation(last_token) && last_token != "(") {
                brackets_stack.clear();
                return false;
            }
        }
        else if (is_operation(token)) {
            if (last_token == "" || is_operation(last_token) || last_token == "(" || is_func(last_token)) {
                brackets_stack.clear();
                return false;
            }
        }
        else if (is_operand(token)) {
            if (last_token != "" && !is_operation(last_token) && last_token != "(") {
                brackets_stack.clear();
                return false;
            }
        }
        else {
            brackets_stack.clear();
            return false;
        }
        last_token = token;
    }

    bool final_result = (brackets_stack.is_empty() && token_count > 0 && !last_token.empty() && !is_operation(last_token) && !is_func(last_token) && last_token != "(");
    brackets_stack.clear();
    return final_result;
}

int get_priority(string op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "^") return 3;
    if (is_func(op)) return 4;
    return 0;
}

string format_double(double n) { // удаляет лишние нули
    stringstream ss;
    ss << n;
    return ss.str();
}

string fix_num_dot(string str) {
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == ',') {
            str[i] = '.';
        }
    }
    return str;
}

double action(string n1, string n2, string op) {

    double a = stod(fix_num_dot(n1));
    double b = stod(fix_num_dot(n2));
    switch (op[0]) {
    case '+':
        return a + b;
        break;
    case '-':
        return a - b;
        break;
    case '*':
        return a * b;
        break;
    case '/':
        if (b == 0) {
            cout << RED << "Ошибка: деление на ноль!" << RESET << endl;
            return 0;
        }
        return a / b;
        break;
    case '^':
        return pow(a, b);
            break;
    default:
        return 0;
        break;
    }
}


// Алгоритм ОПЗ
void convert_to_rpn() {
    cout << RED << "Шаги преобразования в ОПН:\n" << RESET << endl;
    stringstream stream(input_str);
    string token;
    int pointer_step = 0;

    while (stream >> token) {
        if (is_operand(token)) {
            polish_array.push_back(token);

            cout << BLUE << "Ход действий:" << RESET << endl;
            cout << input_str << endl;
            string spaces(pointer_step, ' ');
            cout << spaces << GREEN << "|" << RESET << endl;
            pointer_step += token.length() + 1;
            cout << BLUE << "Текущий символ " << RED << token << BLUE << " является числом. Добавляем в финальную строку." << RESET << endl;
            cout << GREEN << "Текущая строка ОПН: " << RED << polish_array.output() << RESET << endl;
            cout << GREEN << "Стек операций: " << RED;
            stack_op.print();
            stack_steps_waiting();
        }
        else if (is_func(token)) {
            stack_op.push_begin(token);

            cout << BLUE << "Ход действий:" << RESET << endl;
            cout << input_str << endl;
            string spaces(pointer_step, ' ');
            cout << spaces << GREEN << "|" << RESET << endl;
            pointer_step += token.length() + 1;
            cout << BLUE << "Текущий символ " << RED << token << BLUE << " является функцией. Добавляем в стек операций." << RESET << endl;
            cout << GREEN << "Текущая строка ОПН: " << RED << polish_array.output() << RESET << endl;
            cout << GREEN << "Стек операций: " << RED;
            stack_op.print();
            stack_steps_waiting();
        }
        else if (token == "(") {
            stack_op.push_begin(token);

            cout << BLUE << "Ход действий:" << RESET << endl;
            cout << input_str << endl;
            string spaces(pointer_step, ' ');
            cout << spaces << GREEN << "|" << RESET << endl;
            pointer_step += token.length() + 1;
            cout << BLUE << "Текущий символ " << RED << token << BLUE << " является открывающей скобкой. Добавляем в стек операций." << RESET << endl;
            cout << GREEN << "Текущая строка ОПН: " << RED << polish_array.output() << RESET << endl;
            cout << GREEN << "Стек операций: " << RED;
            stack_op.print();
            stack_steps_waiting();
        }
        else if (token == ")") {
            cout << BLUE << "Ход действий:" << RESET << endl;
            cout << input_str << endl;
            string spaces(pointer_step, ' ');
            cout << spaces << GREEN << "|" << RESET << endl;
            pointer_step += token.length() + 1;
            cout << BLUE << "Текущий символ " << RED << token << BLUE << " является закрывающей скобкой. Выводим из стека все операции до открывающей скобки." << RESET << endl;

            while (!stack_op.is_empty() && stack_op.peek() != "(") {
                polish_array.push_back(stack_op.pop());
            }
            if (!stack_op.is_empty()) {
                stack_op.pop();
            }

            if (!stack_op.is_empty() && is_func(stack_op.peek())) {
                polish_array.push_back(stack_op.pop());
            }

            cout << GREEN << "Текущая строка ОПН: " << RED << polish_array.output() << RESET << endl;
            cout << GREEN << "Стек операций: " << RED;
            stack_op.print();
            stack_steps_waiting();
        }
        else if (is_operation(token)) {
            while (!stack_op.is_empty() && ((token == "^" && get_priority(token) < get_priority(stack_op.peek())) || (token != "^" && get_priority(token) <= get_priority(stack_op.peek())))) {
                cout << BLUE << "Ход действий:" << RESET << endl;
                cout << input_str << endl;
                string spaces(pointer_step, ' ');
                cout << spaces << GREEN << "|" << RESET << endl;
                cout << BLUE << "Текущая операция " << RED << token << BLUE << " ниже или равна по приоритету операциям в стеке." << RESET << endl;

                polish_array.push_back(stack_op.pop());

                cout << GREEN << "Текущая строка ОПН: " << RED << polish_array.output() << RESET << endl;
                cout << GREEN << "Стек операций: " << RED;
                stack_op.print();
                stack_steps_waiting();
            }
            stack_op.push_begin(token);

            cout << BLUE << "Ход действий:" << RESET << endl;
            cout << input_str << endl;
            string spaces(pointer_step, ' ');
            cout << spaces << GREEN << "|" << RESET << endl;
            cout << BLUE << "Текущую операцию " << RED << token << BLUE << " добавляем в стек" << RESET << endl;
            cout << GREEN << "Текущая строка ОПН: " << RED << polish_array.output() << RESET << endl;
            cout << GREEN << "Стек операций: " << RED;
            stack_op.print();
            stack_steps_waiting();

            pointer_step += token.length() + 1;
        }
    }

    while (!stack_op.is_empty()) {
        polish_array.push_back(stack_op.pop());
    }

    cout << BLUE << "Выводим все оставшиеся операции из стека в финальную строку." << RESET << endl;
    cout << GREEN << "Текущая строка: " << RED << polish_array.output() << RESET << endl;
    cout << GREEN << "Стек операций: " << RED;
    stack_op.print();
    stack_steps_waiting();
}

void calculate_rpn() {
    clear_screen();
    cout << RED << "Процесс вычисления Обратной Польской Нотации:\n" << RESET << endl;

    stringstream stream(polish_array.output());
    string token;
    int pointer_step = 0;

    while (stream >> token) {
        if (is_operand(token)) {
            stack_calc.push_begin(token);

            cout << BLUE << "Ход действий:" << RESET << endl;
            cout << polish_array.output() << endl;
            string spaces(pointer_step, ' ');
            cout << spaces << GREEN << "|" << RESET << endl;
            pointer_step += token.length() + 1;
            cout << BLUE << "Текущий символ " << RED << token << BLUE << " является числом. Добавляем в стек вычислений." << RESET << endl;
            cout << GREEN << "Стек вычислений: " << RED;
            stack_calc.print();
            stack_steps_waiting();
        }
        else if (is_func(token)) {
            if (stack_calc.is_empty()) {
                cout << RED << "Ошибка: нехватка операндов для функции!" << RESET << endl;
                return;
            }

            cout << BLUE << "Ход действий:" << RESET << endl;
            cout << polish_array.output() << endl;
            string spaces(pointer_step, ' ');
            cout << spaces << GREEN << "|" << RESET << endl;
            cout << BLUE << "Текущий символ " << RED << token << BLUE << " является функцией. Берём одно число из стека." << RESET << endl;
            cout << GREEN << "Стек вычислений: " << RED;
            stack_calc.print();
            stack_steps_waiting();

            string op1 = stack_calc.pop();
            double val = stod(fix_num_dot(op1));
            double temp_res = 0;

            if (token == "sin") temp_res = sin(val);
            else if (token == "cos") temp_res = cos(val);

            stack_calc.push_begin(format_double(temp_res));

            cout << BLUE << "Выполнили " << RED << token << "(" << op1 << ")" << BLUE " и добавили в стек вычислений." << endl;
            cout << GREEN << "Стек вычислений: " << RED;
            stack_calc.print();
            stack_steps_waiting();
            pointer_step += token.length() + 1;
        }
        else if (is_operation(token)) {
            if (stack_calc.size() < 2) {
                cout << RED << "Ошибка: нехватка операндов!" << RESET << endl;
                return;
            }

            cout << BLUE << "Ход действий:" << RESET << endl;
            cout << polish_array.output() << endl;
            string spaces(pointer_step, ' ');
            cout << spaces << GREEN << "|" << RESET << endl;
            cout << BLUE << "Текущий символ " << RED << token << BLUE << " является операцией. Берём два числа из стека." << RESET << endl;
            cout << GREEN << "Стек вычислений: " << RED;
            stack_calc.print();
            stack_steps_waiting();

            string op2 = stack_calc.pop();
            string op1 = stack_calc.pop();

            double temp_res = action(op1, op2, token);

            stack_calc.push_begin(format_double(temp_res));

            cout << BLUE << "Выполнили " << RED << op1 << " " << token << " " << op2 << BLUE " и добавили в стек вычислений." << endl;
            cout << GREEN << "Стек вычислений: " << RED;
            stack_calc.print();
            stack_steps_waiting();
            pointer_step += token.length() + 1;
        }
    }

    cout << RED << "В стеке осталось одно число. Это и есть результат." << RESET << endl;
    cout << GREEN << "Стек вычислений: " << RED;
    stack_calc.print();
    stack_steps_waiting();

    if (!stack_calc.is_empty()) {
        final_result = stod(fix_num_dot(stack_calc.pop()));
        clear_screen();
        cout << GREEN << "Результат вычисления ОПН: " << RESET << final_result << endl;
    }
    waiting();
}


// Функции меню
void convert_polish_menu() {
        clear_screen();
        convert_to_rpn();
        clear_screen();
        cout << GREEN << "Обратная польская нотация готова:\n" << RESET << polish_array.output() << endl;
        waiting();
        calculate_rpn();
        output_simple = true;
        output_rpn = true;
        return;
}

void exp_input_menu() {
    while (true) {
        clear_screen();
        cout << BLUE << "Введите простое выражение (только целые числа и символы через пробел): " << RESET << endl;
        getline(cin, input_str);

        if (input_str.empty()) {
            continue;
        }

        if (is_valid_simple(input_str)) {
            cout << GREEN << "Выражение корректно: " << RESET << input_str << endl;
            waiting();
            convert_polish_menu();
            return;
        }
        else {
            cout << RED << "Ошибка в выражении! Проверьте символы и пробелы." << RESET << endl;
            input_str = "";
            waiting();
        }
    }
}

void output_data() {
    if (output_simple || output_rpn) {
        cout << GREEN << "Текущие данные:" << RESET << endl;
        if (output_simple) {
            cout << BLUE << "Исходное выражение: " << RED << input_str << RESET << endl;
        }
        if (output_rpn) {
            cout << BLUE << "Обратная польская нотация: " << RED << polish_array.output() << RESET << endl;
        }
        cout << BLUE << "Результат выражения: " << RED << final_result << RESET << endl;
    }
    else {
        cout << GREEN << "Пока что нет данных!" << RESET << endl;
    }
}

void command_choice() {
    cout << BLUE << "Введите номер команды: " << RESET;
    int user_choice;
    if (cin >> user_choice) {
        stream_clear();
        menu_commands(user_choice);
    }
    else {
        cout << RED << "Ошибка! Введите номер команды от 0 до 2!\n" << RESET << endl;
        error_detention();
    }
}

void menu_commands(int choice) {
    switch (choice) {
    case 0:
        clear_screen();
        delete_all_data();
        cout << GREEN << "Память очищена, программа завершена!" << RESET << endl;
        exit(0);
    case 1:
        clear_screen();
        delete_all_data();
        exp_input_menu();
        break;
    case 2:
        clear_screen();
        output_data();
        waiting();
        break;
    default:
        cout << RED << "Ошибка! Введите номер команды от 0 до 2!\n" << RESET << endl;
        waiting();
        break;
    }
}

void lab_page() {
    cout << "|=========================================|" << endl;
    cout << "|      " << RED << "Алгоритмы и структура данных" << RESET << "       |" << endl;
    cout << "|          " << BLUE << "Лабораторная работа 1" << RESET << "          |" << endl;
    cout << "|=========================================|" << endl;
    cout << "| " << GREEN << "1) Ввести выражение" << RESET << "                     |" << endl;
    cout << "| " << GREEN << "2) Вывод текущих данных" << RESET << "                 |" << endl;
    cout << "|                                         |" << endl;
    cout << "| " << RED << "0) Выход и очистка памяти" << RESET << "               |" << endl;
    cout << "|=========================================|" << endl;
    cout << endl;
}

// Главная функция
int main() {
    setlocale(0, "");
    setlocale(LC_NUMERIC, "C");
    while (true) {
        lab_page();
        command_choice();
        clear_screen();
    }
    return 0;
}