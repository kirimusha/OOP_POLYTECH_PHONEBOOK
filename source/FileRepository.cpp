#include "include/FileRepository.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <iostream>
#include <sstream>

using namespace std;
using json = nlohmann::json;

// конструктор
// принимает имя файла для хранения контактов
FileRepository::FileRepository(const string& filename) 
    : filename(filename) 
{
    // выводим сообщение о том, какой файл будем использовать
    qDebug() << "[FileRepository] Initialized with file:" << QString::fromStdString(filename);
    
    // загружаем контакты из файла при создании репозитория
    loadFromFile();
}

// деструктор
// сохраняет все контакты в файл перед уничтожением объекта
FileRepository::~FileRepository() 
{
    saveToFile();
    qDebug() << "[FileRepository] Contacts saved and repository closed";
}

// добавление нового контакта
bool FileRepository::addContact(const Contact& contact) 
{
    // проверяем, что контакт валиден
    if (!contact.is_valid()) {
        qDebug() << "[FileRepository] Cannot add invalid contact";
        return false;
    }
    
    // проверяем, что контакта с таким email ещё нет
    if (findContact(contact.get_email()) != nullptr) {
        qDebug() << "[FileRepository] Contact with email" 
                 << QString::fromStdString(contact.get_email()) 
                 << "already exists";
        return false;
    }
    
    // добавляем контакт в список
    contacts.push_back(contact);
    
    // сохраняем изменения в файл
    saveToFile();
    
    qDebug() << "[FileRepository] Contact added:" 
             << QString::fromStdString(contact.get_email());
    return true;
}

// обновление существующего контакта
bool FileRepository::updateContact(const Contact& contact) 
{
    // ищем контакт по email
    Contact* existing = findContact(contact.get_email());
    
    if (existing == nullptr) {
        qDebug() << "[FileRepository] Contact not found for update:" 
                 << QString::fromStdString(contact.get_email());
        return false;
    }
    
    // проверяем валидность нового контакта
    if (!contact.is_valid()) {
        qDebug() << "[FileRepository] Cannot update with invalid contact data";
        return false;
    }
    
    // обновляем данные
    *existing = contact;
    
    // сохраняем изменения в файл
    saveToFile();
    
    qDebug() << "[FileRepository] Contact updated:" 
             << QString::fromStdString(contact.get_email());
    return true;
}

// удаление контакта по email
bool FileRepository::removeContact(const string& email) 
{
    // используем std::remove_if для удаления контакта
    auto it = std::remove_if(contacts.begin(), contacts.end(),
        [&email](const Contact& c) {
            return c.get_email() == email;
        });
    
    // если контакт не найден
    if (it == contacts.end()) {
        qDebug() << "[FileRepository] Contact not found for removal:" 
                 << QString::fromStdString(email);
        return false;
    }
    
    // удаляем найденный контакт
    contacts.erase(it, contacts.end());
    
    // сохраняем изменения в файл
    saveToFile();
    
    qDebug() << "[FileRepository] Contact removed:" << QString::fromStdString(email);
    return true;
}

// получение контакта по email
Contact FileRepository::getContact(const string& email) const 
{
    Contact* found = findContact(email);
    
    if (found != nullptr) {
        return *found;
    }
    
    // если не найден, возвращаем пустой контакт
    qDebug() << "[FileRepository] Contact not found:" << QString::fromStdString(email);
    return Contact();
}

// получение всех контактов
vector<Contact> FileRepository::getAllContacts() const 
{
    return contacts;
}

// обновление всех контактов сразу (например, после сортировки)
void FileRepository::updateAllContacts(const vector<Contact>& newContacts) 
{
    contacts = newContacts;
    saveToFile();
    qDebug() << "[FileRepository] All contacts updated, total:" << contacts.size();
}

// ПРИВАТНЫЕ МЕТОДЫ

// поиск контакта по email (возвращает указатель для возможности модификации)
Contact* FileRepository::findContact(const string& email) const 
{
    for (auto& contact : contacts) {
        if (contact.get_email() == email) {
            return const_cast<Contact*>(&contact);
        }
    }
    return nullptr;
}

// загрузка контактов из файла
void FileRepository::loadFromFile() 
{
    // создаем объект QFile для работы с файлом
    QFile file(QString::fromStdString(filename));
    
    // пытаемся открыть файл для чтения
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // если файл не существует - это нормально (первый запуск)
        qDebug() << "[FileRepository] File does not exist yet:" 
                 << QString::fromStdString(filename) 
                 << "- creating new one";
        return;
    }
    
    // читаем весь файл в QString
    QTextStream in(&file);
    QString jsonContent = in.readAll();
    file.close();
    
    // если файл пустой
    if (jsonContent.isEmpty()) {
        qDebug() << "[FileRepository] File is empty";
        return;
    }
    
    try {
        // парсим JSON
        json data = json::parse(jsonContent.toStdString());
        
        // проверяем, что это массив
        if (!data.is_array()) {
            qDebug() << "[FileRepository] Invalid JSON format: expected array";
            return;
        }
        
        // очищаем текущий список контактов
        contacts.clear();
        
        // загружаем каждый контакт
        for (const auto& item : data) {
            Contact contact = Contact::fromJson(item.dump());
            
            // добавляем только валидные контакты
            if (contact.is_valid()) {
                contacts.push_back(contact);
            } else {
                qDebug() << "[FileRepository] Skipping invalid contact during load";
            }
        }
        
        qDebug() << "[FileRepository] Loaded" << contacts.size() << "contacts from file";
        
    } catch (const json::exception& e) {
        qDebug() << "[FileRepository] JSON parse error:" << e.what();
    }
}

// сохранение контактов в файл
void FileRepository::saveToFile() 
{
    try {
        // создаем JSON массив
        json data = json::array();
        
        // добавляем все контакты в массив
        for (const auto& contact : contacts) {
            data.push_back(contact.toJsonObj());
        }
        
        // преобразуем в красиво отформатированную строку (с отступами)
        string jsonStr = data.dump(4);  // 4 - количество пробелов для отступа
        
        // создаем объект QFile для записи
        QFile file(QString::fromStdString(filename));
        
        // открываем файл для записи (WriteOnly - только запись, Truncate - очистить содержимое)
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            qDebug() << "[FileRepository] Cannot open file for writing:" 
                     << QString::fromStdString(filename);
            return;
        }
        
        // создаем поток для записи текста
        QTextStream out(&file);
        
        // записываем JSON в файл
        out << QString::fromStdString(jsonStr);
        
        // закрываем файл
        file.close();
        
        qDebug() << "[FileRepository] Saved" << contacts.size() 
                 << "contacts to file:" << QString::fromStdString(filename);
        
    } catch (const json::exception& e) {
        qDebug() << "[FileRepository] Error during save:" << e.what();
    }
}

// поиск контактов по имени (для функции поиска)
vector<Contact> FileRepository::searchByName(const string& query) const 
{
    vector<Contact> results;
    
    // приводим запрос к нижнему регистру для поиска без учета регистра
    string lowerQuery = query;
    transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    // ищем контакты, в которых имя, фамилия или отчество содержат запрос
    for (const auto& contact : contacts) {
        // получаем имя, фамилию и отчество
        string firstName = contact.get_firstName();
        string lastName = contact.get_lastName();
        string patronymic = contact.get_patronymic();
        
        // приводим к нижнему регистру
        transform(firstName.begin(), firstName.end(), firstName.begin(), ::tolower);
        transform(lastName.begin(), lastName.end(), lastName.begin(), ::tolower);
        transform(patronymic.begin(), patronymic.end(), patronymic.begin(), ::tolower);
        
        // проверяем, содержит ли хотя бы одно из полей наш запрос
        if (firstName.find(lowerQuery) != string::npos ||
            lastName.find(lowerQuery) != string::npos ||
            patronymic.find(lowerQuery) != string::npos) {
            results.push_back(contact);
        }
    }
    
    qDebug() << "[FileRepository] Search for '" << QString::fromStdString(query) 
             << "' found" << results.size() << "contacts";
    
    return results;
}