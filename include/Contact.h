#ifndef CONTACT_H
#define CONTACT_H

#include <string>
#include <list>
#include "PhoneNumber.h"
#include "../third_party/json.hpp"

using namespace std;

// Режим работы класса Contact
enum class ContactMode {
    CONSOLE,  // режим консольного приложения (с интерактивным вводом)
    GUI       // режим GUI приложения (без интерактивного ввода)
};

class Contact {
private:
    string firstName;
    string lastName;
    string patronymic;
    string address;
    string birthDate;
    string email;
    list<PhoneNumber> phones;
    
    // текущий режим работы (по умолчанию консольный для обратной совместимости)
    static ContactMode workMode;

public:
    // конструкторы
    Contact(const string& firstName = "",
            const string& lastName = "", 
            const string& patronymic = "",
            const string& address = "",
            const string& birthDate = "",
            const string& email = "",
            const list<PhoneNumber> phones = {});
    
    ~Contact();
    
    // конструктор копирования
    Contact(const Contact& other);
    
    // оператор присваивания
    Contact& operator=(const Contact& other);

    // установка режима работы (статический метод)
    static void setMode(ContactMode mode);
    static ContactMode getMode();

    // геттеры
    string get_firstName() const;
    string get_lastName() const;
    string get_patronymic() const;
    string get_address() const;
    string get_birthDate() const;
    string get_email() const;
    list<PhoneNumber> get_phones() const;

    // сеттеры
    bool set_firstName(const string& newFirstName);
    bool set_lastName(const string& newLastName);
    bool set_patronymic(const string& newPatronymic);
    void set_address(const string& newAddress);
    bool set_birthDate(const string& newBirthDate);
    bool set_email(const string& newEmail);
    bool set_phones(const list<PhoneNumber>& newPhones);

    // работа с телефонами
    void addPhone(const PhoneNumber& phone);
    void removePhone(int index);
    void clearPhones();
    int phoneCount() const;

    // валидация
    bool is_valid() const;
    static bool validate_name(const string& name);
    static bool validate_email(const string& email);
    static bool validate_birthDate(const string& date);

    // сериализация
    nlohmann::json toJsonObj() const;
    string toJson() const;
    static Contact fromJson(const string& jsonStr);

    // вспомогательные методы
    string toString() const;
    
    // операторы сравнения
    bool operator==(const Contact& other) const;
    bool operator!=(const Contact& other) const;

private:
    // вспомогательный метод для запроса телефонов (только для консольного режима)
    list<PhoneNumber> prompt_for_phones();
};

#endif