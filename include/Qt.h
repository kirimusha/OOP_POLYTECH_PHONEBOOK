#ifndef QT
#define QT

// Подключаем базовый класс главного окна Qt
#include <QMainWindow>

// Подключаем виджеты Qt для построения интерфейса
#include <QTableWidget>     // Таблица для отображения контактов
#include <QLineEdit>        // Однострочное текстовое поле (для имени, email и т.д.)
#include <QTextEdit>        // Многострочное текстовое поле (для адреса)
#include <QPushButton>      // Кнопка
#include <QVBoxLayout>      // Вертикальный менеджер компоновки
#include <QHBoxLayout>      // Горизонтальный менеджер компоновки
#include <QFormLayout>      // Компоновка в виде формы (метка: поле)
#include <QGroupBox>        // Группа виджетов с рамкой и заголовком
#include <QLabel>           // Текстовая метка
#include <QMessageBox>      // Диалоговое окно для сообщений
#include <QHeaderView>      // Для работы с заголовками таблицы

// Подключаем уже существующие классы
#include "include/ContactManager.h"   // Менеджер для работы с контактами
#include "include/FileRepository.h"   // Репозиторий для сохранения в файл

/*
 Класс QtPhoneBook - главное окно приложения телефонной книги
 Наследуется от QMainWindow - базового класса для главных окон в Qt
 */
class QtPhoneBook : public QMainWindow
{
    Q_OBJECT  // Макрос Qt, необходимый для работы системы сигналов и слотов

public:
    /*
     Конструктор главного окна
     parent - указатель на родительский виджет (по умолчанию nullptr)
    */
    QtPhoneBook(QWidget *parent = nullptr);
    
    // Деструктор - освобождает выделенную память
    ~QtPhoneBook();

private slots:
    // СЛОТЫ - специальные методы Qt, которые могут быть вызваны сигналами
    // Это обработчики событий пользовательского интерфейса
    
    /*
    Обработчик добавления нового контакта
    Вызывается при нажатии кнопки "Add Contact"
    */
    void onAddContact();
    
    /*
    Обработчик редактирования контакта
    Вызывается при нажатии кнопки "Edit Contact"
     */
    void onEditContact();
    
    /*
    Обработчик удаления контакта
    Вызывается при нажатии кнопки "Delete Contact"
    */
    void onDeleteContact();
    
    /*
    Обработчик поиска контактов
    Вызывается при нажатии кнопки "Search" или клавиши Enter в поле поиска
    */
    void onSearchContacts();
    
    /*
    Обработчик выбора контакта из таблицы
    Вызывается при клике на строку таблицы
    row - номер выбранной строки
    column - номер столбца (не используется, но нужен для сигнала)
     */
    void onContactSelected(int row, int column);
    
    /*
    Обработчик очистки формы
    Вызывается при нажатии кнопки "Clear Form"
    */
    void onClearForm();

private:  
    /*
    Инициализация пользовательского интерфейса
    Создает все виджеты, размещает их и подключает сигналы к слотам
     */
    void setupUI();
    
    /*
    Обновление таблицы контактов
    Очищает таблицу и заполняет её актуальными данными из менеджера
     */
    void refreshContactTable();
    
    /*
    Отображение данных контакта в форме редактирования
    contact - контакт для отображения
    */
    void displayContact(const Contact& contact);
    
    /*
    Получение контакта из полей формы
    Считывает данные из всех текстовых полей и создает объект Contact
    возвращает объект Contact с данными из формы
     */
    Contact getContactFromForm() const;
    
    /*
    Валидация данных контакта
    Проверяет корректность всех полей и возвращает сообщение об ошибке
    contact - контакт для проверки
    возвращает QString с описанием ошибок, или пустую строку если все ok
     */
    QString validateContactData(const Contact& contact) const;

    // Вспомогательный метод для форматирования списка телефонов
    QString formatPhones(const std::list<PhoneNumber>& phones) const;

    // данные класса
    
    /*
    Указатель на менеджер контактов
    Отвечает за всю логику: добавление, удаление, поиск контактов
    */
    ContactManager *manager;
    
    // UI элементы - указатели на виджеты интерфейса
    
    /*
    Таблица контактов (левая панель)
    Отображает все контакты в виде таблицы с возможностью сортировки
    Столбцы: First Name | Last Name | Patronymic | Email | Birth Date
    */
    QTableWidget *contactsTable;
    
    /*
    Поле для поиска контактов по имени
    */
    QLineEdit *searchEdit;
    
    // поля формы редактирования контакта, ну, или правая панель
    
    QLineEdit *firstNameEdit;    // Поле "Имя"
    QLineEdit *lastNameEdit;     // Поле "Фамилия"
    QLineEdit *patronymicEdit;   // Поле "Отчество"
    QLineEdit *emailEdit;        // Поле "Email"
    QLineEdit *birthDateEdit;    // Поле "Дата рождения"
    QTextEdit *addressEdit;      // Многострочное поле "Адрес"
    
    // кнопки управления
    
    QPushButton *addButton;      // Кнопка "Add Contact"
    QPushButton *editButton;     // Кнопка "Edit Contact"
    QPushButton *deleteButton;   // Кнопка "Delete Contact"
    QPushButton *searchButton;   // Кнопка "Search"
    QPushButton *clearButton;    // Кнопка "Clear Form"
};

#endif