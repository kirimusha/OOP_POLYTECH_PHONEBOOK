#ifndef FILEREPOSITORY_H
#define FILEREPOSITORY_H

#include "IContactRepository.h"
#include <string>
#include <vector>

using namespace std;

/*
 Класс FileRepository - реализация репозитория контактов с хранением в файле
 Наследуется от интерфейса IContactRepository
 Использует JSON формат для сериализации данных
 */
class FileRepository : public IContactRepository {
private:
    string filename;              // имя файла для хранения контактов
    vector<Contact> contacts;     // список всех контактов в памяти
    
    /*
     Приватный метод для поиска контакта по email
     Возвращает указатель на контакт или nullptr если не найден
     */
    Contact* findContact(const string& email) const;
    
    /*
     Загрузка контактов из файла
     Вызывается автоматически в конструкторе
     */
    void loadFromFile();
    
    /*
     Сохранение контактов в файл
     Вызывается после каждого изменения данных
     */
    void saveToFile();

public:
    /*
     Конструктор
     filename - имя файла для хранения контактов
     */
    FileRepository(const string& filename);
    
    /*
     Деструктор
     Сохраняет все изменения в файл перед закрытием
     */
    ~FileRepository();

    // Реализация методов интерфейса IContactRepository
    
    /*
     Добавление нового контакта
     Возвращает true если контакт успешно добавлен
     */
    bool addContact(const Contact& contact) override;
    
    /*
     Обновление существующего контакта
     Поиск осуществляется по email
     */
    bool updateContact(const Contact& contact) override;
    
    /*
     Удаление контакта по email
     */
    bool removeContact(const string& email) override;
    
    /*
     Получение контакта по email
     Если контакт не найден, возвращает пустой объект Contact
     */
    Contact getContact(const string& email) const override;
    
    /*
     Получение списка всех контактов
     */
    vector<Contact> getAllContacts() const override;
    
    /*
     Обновление всего списка контактов
     Используется, например, после сортировки
     */
    void updateAllContacts(const vector<Contact>& newContacts);
    
    /*
     Поиск контактов по имени (first name, last name, patronymic)
     Поиск регистронезависимый
     */
    vector<Contact> searchByName(const string& query) const;
};

#endif