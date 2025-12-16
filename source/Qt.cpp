#include "include/Qt.h"
#include "ui_mainwindow.h"

/*
    конструктор класса QtPhoneBook
    вызывается при создании объекта окна приложения

    parent - указатель на родительский виджет
*/
QtPhoneBook::QtPhoneBook(QWidget *parent)
    : QMainWindow(parent)  // вызываем конструктор базового класса QMainWindow
{
    // устанавливаем режим GUI для класса Contact
    // это отключает интерактивный ввод из консоли
    Contact::setMode(ContactMode::GUI);
    
    // создаем менеджер контактов с файловым репозиторием
    // FileRepository будет сохранять данные в файл "contacts.json"
    manager = new ContactManager(new FileRepository("contacts.json"));
    
    // инициализируем пользовательский интерфейс
    // создаем все виджеты и размещаем их в окне
    setupUI();
    
    // загружаем и отображаем все контакты из файла
    refreshContactTable();
}

/*
    деструктор класса QtPhoneBook
    вызывается при уничтожении объекта окна
    освобождает память, выделенную под менеджер контактов
 */
QtPhoneBook::~QtPhoneBook()
{
    delete manager;  // удаляем менеджер (он автоматически удалит и репозиторий)
}

/*
    метод setupUI
    создает и настраивает весь пользовательский интерфейс приложения
    теперь использует QTableWidget вместо QListWidget
*/
void QtPhoneBook::setupUI()
{
    // устанавливаем заголовок окна
    setWindowTitle("PHONEBOOK");
    
    // устанавливаем минимальный размер окна (1000x600 пикселей - шире для таблицы)
    setMinimumSize(1000, 600);

    // создаем центральный виджет окна
    // в QMainWindow все содержимое должно быть помещено в центральный виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // создаем главный горизонтальный layout (менеджер компоновки)
    // он будет содержать левую и правую панели
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);


    // ЛЕВАЯ ПАНЕЛЬ - ТЕПЕРЬ С ТАБЛИЦЕЙ


    // создаем вертикальный layout для левой панели (таблица контактов)
    QVBoxLayout *leftLayout = new QVBoxLayout();
    
    // метка "Contacts:"
    QLabel *contactsLabel = new QLabel("Contacts:");
    
    // создаем горизонтальный layout для строки поиска
    QHBoxLayout *searchLayout = new QHBoxLayout();
    
    // создаем поле ввода для поиска
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Search by name...");  // подсказка в пустом поле
    
    // создаем кнопку поиска
    searchButton = new QPushButton("Search");
    
    // добавляем поле и кнопку в горизонтальный layout
    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(searchButton);
    
    // создаем таблицу для отображения контактов
    contactsTable = new QTableWidget();

    // Или для переноса текста в ячейках:
    contactsTable->setWordWrap(true);
    contactsTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    
    // настраиваем таблицу
    // устанавливаем количество столбцов (6 штук)
    contactsTable->setColumnCount(6);
    
    // устанавливаем заголовки столбцов
    QStringList headers;
    headers << "First Name" << "Last Name" << "Patronymic" << "Email" << "Birth Date" << "Phones";
    contactsTable->setHorizontalHeaderLabels(headers);
    
    // настраиваем поведение таблицы
    contactsTable->setSelectionBehavior(QAbstractItemView::SelectRows);  // выбирать всю строку
    contactsTable->setSelectionMode(QAbstractItemView::SingleSelection);  // только одна строка
    contactsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);    // запрещаем редактирование в таблице
    
    // настраиваем размеры столбцов
    // последний столбец растягивается, остальные подстраиваются под содержимое
    contactsTable->horizontalHeader()->setStretchLastSection(true);
    contactsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    
    // включаем сортировку по столбцам (клик на заголовок = сортировка)
    contactsTable->setSortingEnabled(true);
    
    // делаем красивые полосы (чередующиеся цвета строк)
    contactsTable->setAlternatingRowColors(true);
    
    // собираем левую панель: метка, строка поиска, таблица контактов
    leftLayout->addWidget(contactsLabel);
    leftLayout->addLayout(searchLayout);
    leftLayout->addWidget(contactsTable);


    // ПРАВАЯ ПАНЕЛЬ


    // создаем вертикальный layout для правой панели (форма редактирования)
    QVBoxLayout *rightLayout = new QVBoxLayout();
    
    // создаем группу виджетов с рамкой и заголовком "Contact Details"
    QGroupBox *formGroup = new QGroupBox("Contact Details");
    
    // создаем форму (layout типа "метка: поле")
    QFormLayout *formLayout = new QFormLayout();
    
    // создаем все текстовые поля для ввода данных контакта
    firstNameEdit = new QLineEdit();
    lastNameEdit = new QLineEdit();
    patronymicEdit = new QLineEdit();
    emailEdit = new QLineEdit();
    birthDateEdit = new QLineEdit();
    birthDateEdit->setPlaceholderText("DD.MM.YYYY");  // подсказка формата даты
    
    // многострочное поле для адреса
    addressEdit = new QTextEdit();
    addressEdit->setMaximumHeight(100);  // ограничиваем высоту
    
    // добавляем строки в форму (метка: поле)
    // звездочка (*) означает обязательное поле
    formLayout->addRow("First Name *:", firstNameEdit);
    formLayout->addRow("Last Name *:", lastNameEdit);
    formLayout->addRow("Patronymic:", patronymicEdit);
    formLayout->addRow("Email *:", emailEdit);
    formLayout->addRow("Birth Date:", birthDateEdit);
    formLayout->addRow("Address:", addressEdit);
    
    // устанавливаем layout для группы
    formGroup->setLayout(formLayout);
    

    // КНОПКИ УПРАВЛЕНИЯ 


    // создаем горизонтальный layout для кнопок
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    // создаем все кнопки управления контактами
    addButton = new QPushButton("Add Contact");
    editButton = new QPushButton("Edit Contact");
    deleteButton = new QPushButton("Delete Contact");
    clearButton = new QPushButton("Clear Form");
    
    // добавляем кнопки в горизонтальный layout
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(clearButton);
    
    // собираем правую панель
    rightLayout->addWidget(formGroup);     // форма редактирования
    rightLayout->addLayout(buttonLayout);  // кнопки управления
    rightLayout->addStretch();             // растягивающийся элемент (занимает свободное место)


    // СБОРКА ГЛАВНОГО LAYOUT


    // добавляем левую панель с пропорцией 2 (шире, т.к. таблица)
    mainLayout->addLayout(leftLayout, 2);
    // добавляем правую панель с пропорцией 1
    mainLayout->addLayout(rightLayout, 1);


    // ПОДКЛЮЧЕНИЕ СИГНАЛОВ К СЛОТАМ 


    // это механизм Qt для обработки событий
    // формат: connect(источник_сигнала, &Класс::сигнал, получатель, &Класс::слот)
    
    // когда пользователь кликает на ячейку таблицы, вызывается onContactSelected
    connect(contactsTable, &QTableWidget::cellClicked, this, &QtPhoneBook::onContactSelected);
    
    // Подключаем кнопки к соответствующим обработчикам
    connect(addButton, &QPushButton::clicked, this, &QtPhoneBook::onAddContact);
    connect(editButton, &QPushButton::clicked, this, &QtPhoneBook::onEditContact);
    connect(deleteButton, &QPushButton::clicked, this, &QtPhoneBook::onDeleteContact);
    connect(searchButton, &QPushButton::clicked, this, &QtPhoneBook::onSearchContacts);
    connect(clearButton, &QPushButton::clicked, this, &QtPhoneBook::onClearForm);
    
    // Подключаем поиск при нажатии Enter в поле поиска
    connect(searchEdit, &QLineEdit::returnPressed, this, &QtPhoneBook::onSearchContacts);
}

/*
 * Вспомогательный метод для форматирования телефонов в читаемую строку
 * Формат: "номер (тип), номер (тип), ..."
 * 
 * phones - список телефонов контакта
 * возвращает QString с отформатированными телефонами
 */
QString QtPhoneBook::formatPhones(const std::list<PhoneNumber>& phones) const
{
    // Если список пуст, возвращаем пустую строку
    if (phones.empty()) {
        return "";
    }
    
    QStringList formattedPhones;
    
    // Проходим по всем телефонам в контакте
    for (const auto& phone : phones) {
        // Получаем номер телефона
        QString phoneNumber = QString::fromStdString(phone.get_number());
        
        // Получаем тип телефона и преобразуем в читаемую строку
        QString phoneType;
        switch (phone.get_type()) {
            case PhoneType::Mobile:
                phoneType = "Mobile";
                break;
            case PhoneType::Home:
                phoneType = "Home";
                break;
            case PhoneType::Work:
                phoneType = "Work";
                break;
            default:
                phoneType = "Other";
                break;
        }
        
        // Форматируем: "номер (тип)"
        formattedPhones << phoneNumber + " (" + phoneType + ")";
    }
    
    // Объединяем все телефоны через запятую
    return formattedPhones.join(", ");
}

/**
 * МЕТОД refreshContactTable
 * Обновляет таблицу контактов
 * Загружает все контакты из менеджера и отображает их в таблице
 */
void QtPhoneBook::refreshContactTable()
{
    // временно отключаем сортировку
    contactsTable->setSortingEnabled(false);
    
    // Очищаем таблицу
    contactsTable->setRowCount(0);
    
    // Получаем все контакты
    auto contacts = manager->getAllContacts();
    
    // Проходим по каждому контакту
    int row = 0;
    for (const auto& contact : contacts) {
        // добавляем новую строку
        contactsTable->insertRow(row);
        
        // Столбцы 0-4: существующие поля
        contactsTable->setItem(row, 0, new QTableWidgetItem(
            QString::fromStdString(contact.get_firstName())));
        contactsTable->setItem(row, 1, new QTableWidgetItem(
            QString::fromStdString(contact.get_lastName())));
        contactsTable->setItem(row, 2, new QTableWidgetItem(
            QString::fromStdString(contact.get_patronymic())));
        contactsTable->setItem(row, 3, new QTableWidgetItem(
            QString::fromStdString(contact.get_email())));
        contactsTable->setItem(row, 4, new QTableWidgetItem(
            QString::fromStdString(contact.get_birthDate())));
        
        // Столбец 5: телефоны (новый)
        QString phonesText = formatPhones(contact.get_phones());
        QTableWidgetItem *phonesItem = new QTableWidgetItem(phonesText);
        
        // Делаем подсказку с телефонами (полезно при длинных списках)
        phonesItem->setToolTip(phonesText);
        
        contactsTable->setItem(row, 5, phonesItem);
        
        row++;
    }
    
    // включаем обратно сортировку
    contactsTable->setSortingEnabled(true);
    
    // Автоматически подстраиваем ширину столбцов
    contactsTable->resizeColumnsToContents();
    
    // Устанавливаем минимальную ширину для столбца с телефонами
    contactsTable->horizontalHeader()->setMinimumSectionSize(150);
}

/**
 * СЛОТ onContactSelected
 * Вызывается, когда пользователь кликает на строку в таблице
 * Загружает данные выбранного контакта в форму редактирования
 * 
 * @param row - номер выбранной строки
 * @param column - номер столбца (не используется)
 */
void QtPhoneBook::onContactSelected(int row, int column)
{
    // игнорируем параметр column, он нам не нужен
    Q_UNUSED(column);
    
    // получаем email из третьего столбца (индекс 3)
    QTableWidgetItem *emailItem = contactsTable->item(row, 3);
    
    // проверяем, что ячейка существует
    if (!emailItem) {
        return;
    }
    
    // получаем email
    QString email = emailItem->text();
    
    // Получаем полный объект контакта по email из менеджера
    Contact contact = manager->getContact(email.toStdString());
    
    // Отображаем данные контакта в форме
    displayContact(contact);
}

/**
 * МЕТОД displayContact
 * Заполняет поля формы данными из объекта Contact
 * 
 * @param contact - контакт для отображения
 */
void QtPhoneBook::displayContact(const Contact& contact)
{
    // Преобразуем std::string в QString и заполняем поля
    firstNameEdit->setText(QString::fromStdString(contact.get_firstName()));
    lastNameEdit->setText(QString::fromStdString(contact.get_lastName()));
    patronymicEdit->setText(QString::fromStdString(contact.get_patronymic()));
    emailEdit->setText(QString::fromStdString(contact.get_email()));
    birthDateEdit->setText(QString::fromStdString(contact.get_birthDate()));
    addressEdit->setPlainText(QString::fromStdString(contact.get_address()));
}

/**
 * МЕТОД getContactFromForm
 * Создает объект Contact на основе данных из полей формы
 * 
 * @return новый объект Contact с данными из формы
 */
Contact QtPhoneBook::getContactFromForm() const
{
    // Создаем пустой объект контакта
    Contact contact;
    
    // Заполняем его данными из полей формы
    // Преобразуем QString в std::string методом toStdString()
    contact.set_firstName(firstNameEdit->text().toStdString());
    contact.set_lastName(lastNameEdit->text().toStdString());
    contact.set_patronymic(patronymicEdit->text().toStdString());
    contact.set_email(emailEdit->text().toStdString());
    contact.set_birthDate(birthDateEdit->text().toStdString());
    contact.set_address(addressEdit->toPlainText().toStdString());
    
    // Пока оставляем пример с одним телефоном
    std::list<PhoneNumber> phones;
    
    // Пример для нескольких телефонов:
    // Если у вас есть виджеты для ввода телефонов (например, phone1Edit, phone1TypeCombo и т.д.)
    // if (!phone1Edit->text().isEmpty()) {
    //     PhoneType type1 = static_cast<PhoneType>(phone1TypeCombo->currentIndex());
    //     phones.push_back(PhoneNumber(phone1Edit->text().toStdString(), type1));
    // }
    // if (!phone2Edit->text().isEmpty()) {
    //     PhoneType type2 = static_cast<PhoneType>(phone2TypeCombo->currentIndex());
    //     phones.push_back(PhoneNumber(phone2Edit->text().toStdString(), type2));
    // }
    
    // Временно оставляем один тестовый телефон
    if (phones.empty()) {
        phones.push_back(PhoneNumber("+79123456789", PhoneType::Mobile));
    }
    
    contact.set_phones(phones);
    
    return contact;
}

/**
 * МЕТОД validateContactData
 * Проверяет валидность данных контакта и возвращает сообщение об ошибке
 * 
 * @param contact - контакт для проверки
 * @return QString с описанием ошибки, или пустая строка если все ok
 */
QString QtPhoneBook::validateContactData(const Contact& contact) const
{
    // собираем все ошибки валидации в одну строку
    QStringList errors;
    
    // проверяем First Name
    if (contact.get_firstName().empty()) {
        errors << "• First Name is required (cannot be empty)";
    } else if (!Contact::validate_name(contact.get_firstName())) {
        errors << "• First Name is invalid: \"" + QString::fromStdString(contact.get_firstName()) + 
                  "\" (only letters are allowed)";
    }
    
    // проверяем Last Name
    if (contact.get_lastName().empty()) {
        errors << "• Last Name is required (cannot be empty)";
    } else if (!Contact::validate_name(contact.get_lastName())) {
        errors << "• Last Name is invalid: \"" + QString::fromStdString(contact.get_lastName()) + 
                  "\" (only letters are allowed)";
    }
    
    // проверяем Patronymic (если не пустое)
    if (!contact.get_patronymic().empty() && !Contact::validate_name(contact.get_patronymic())) {
        errors << "• Patronymic is invalid: \"" + QString::fromStdString(contact.get_patronymic()) + 
                  "\" (only letters are allowed)";
    }
    
    // проверяем Email
    if (contact.get_email().empty()) {
        errors << "• Email is required (cannot be empty)";
    } else if (!Contact::validate_email(contact.get_email())) {
        errors << "• Email is invalid: \"" + QString::fromStdString(contact.get_email()) + 
                  "\" (required format: user@domain.com)";
    }
    
    // проверяем Birth Date (если указана)
    if (!contact.get_birthDate().empty() && !Contact::validate_birthDate(contact.get_birthDate())) {
        errors << "• Birth Date is invalid: \"" + QString::fromStdString(contact.get_birthDate()) + 
                  "\" (required format: DD.MM.YYYY)";
    }
    
    // проверяем телефоны
    if (contact.get_phones().empty()) {
        errors << "• At least one phone number is required";
    } else {
        for (const auto& phone : contact.get_phones()) {
            if (!phone.is_valid()) {
                errors << "• Phone number is invalid: \"" + 
                          QString::fromStdString(phone.get_number()) + "\"";
            }
        }
    }
    
    // если есть ошибки, возвращаем их списком
    if (!errors.isEmpty()) {
        return "Validation errors:\n\n" + errors.join("\n");
    }
    
    return "";  // все ok
}

/**
 * СЛОТ onAddContact
 * Обработчик добавления нового контакта
 * Вызывается при нажатии кнопки "Add Contact"
 */
void QtPhoneBook::onAddContact()
{
    // Получаем данные контакта из формы
    Contact contact = getContactFromForm();
    
    // Проверяем валидность данных и получаем сообщение об ошибке
    QString validationError = validateContactData(contact);
    
    if (!validationError.isEmpty()) {
        // Если есть ошибки валидации, показываем их пользователю
        QMessageBox::warning(this, "Validation Error", validationError);
        return;  // Прерываем выполнение метода
    }
    
    // Пытаемся добавить контакт через менеджер
    if (manager->addContact(contact)) {
        // Если успешно добавлен
        refreshContactTable();  // Обновляем таблицу контактов
        onClearForm();          // Очищаем форму
        // Показываем сообщение об успехе
        QMessageBox::information(this, "Success", "Contact added successfully!");
    } else {
        // Если не удалось добавить (например, email уже существует)
        QMessageBox::warning(this, "Error", 
            "Contact with this email already exists!\n\nPlease use a different email address.");
    }
}

/**
 * СЛОТ onEditContact
 * Обработчик редактирования существующего контакта
 * Вызывается при нажатии кнопки "Edit Contact"
 */
void QtPhoneBook::onEditContact()
{
    // Получаем текущую выбранную строку таблицы
    int currentRow = contactsTable->currentRow();
    
    // Если ничего не выбрано (currentRow = -1), показываем предупреждение
    if (currentRow < 0) {
        QMessageBox::warning(this, "Error", "Please select a contact to edit.");
        return;
    }
    
    // Получаем обновленные данные из формы
    Contact updatedContact = getContactFromForm();
    
    // Проверяем валидность данных
    QString validationError = validateContactData(updatedContact);
    
    if (!validationError.isEmpty()) {
        // Если есть ошибки валидации, показываем их
        QMessageBox::warning(this, "Validation Error", validationError);
        return;
    }
    
    // Обновляем контакт через менеджер
    if (manager->updateContact(updatedContact)) {
        // Если успешно обновлен
        refreshContactTable();  // Обновляем таблицу
        QMessageBox::information(this, "Success", "Contact updated successfully!");
    } else {
        // Если не удалось обновить
        QMessageBox::warning(this, "Error", 
            "Failed to update contact!\n\nMake sure the contact still exists.");
    }
}

/**
 * СЛОТ onDeleteContact
 * Обработчик удаления контакта
 * Вызывается при нажатии кнопки "Delete Contact"
 */
void QtPhoneBook::onDeleteContact()
{
    // Получаем текущую выбранную строку таблицы
    int currentRow = contactsTable->currentRow();
    
    // Если ничего не выбрано, показываем предупреждение
    if (currentRow < 0) {
        QMessageBox::warning(this, "Error", "Please select a contact to delete.");
        return;
    }
    
    // получаем email из третьего столбца (индекс 3)
    QTableWidgetItem *emailItem = contactsTable->item(currentRow, 3);
    if (!emailItem) {
        return;
    }
    
    QString email = emailItem->text();
    
    // получаем имя контакта для отображения в диалоге
    QString firstName = contactsTable->item(currentRow, 0)->text();
    QString lastName = contactsTable->item(currentRow, 1)->text();
    QString contactName = firstName + " " + lastName;
    
    // Показываем диалог подтверждения удаления
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Delete", 
                                 "Are you sure you want to delete this contact?\n\n" + 
                                 contactName + "\n" + email,
                                 QMessageBox::Yes | QMessageBox::No);
    
    // Если пользователь подтвердил удаление
    if (reply == QMessageBox::Yes) {
        // Удаляем контакт через менеджер
        if (manager->removeContact(email.toStdString())) {
            // Если успешно удален
            refreshContactTable();  // Обновляем таблицу
            onClearForm();          // Очищаем форму
            QMessageBox::information(this, "Success", 
                "Contact deleted successfully!\n\n" + contactName);
        } else {
            // Если не удалось удалить
            QMessageBox::warning(this, "Error", 
                "Failed to delete contact!\n\nThe contact may have already been deleted.");
        }
    }
}

/**
 * СЛОТ onSearchContacts
 * Обработчик поиска контактов по имени
 * Вызывается при нажатии кнопки "Search" или клавиши Enter в поле поиска
 */
void QtPhoneBook::onSearchContacts()
{
    // Получаем поисковый запрос из поля ввода и убираем пробелы по краям
    QString query = searchEdit->text().trimmed();
    
    // Если запрос пустой, показываем все контакты
    if (query.isEmpty()) {
        refreshContactTable();
        return;
    }
    
    // временно отключаем сортировку
    contactsTable->setSortingEnabled(false);
    
    // Очищаем таблицу
    contactsTable->setRowCount(0);
    
    // Выполняем поиск через менеджер
    auto results = manager->searchByName(query.toStdString());
    
    // Отображаем найденные контакты
    int row = 0;
    for (const auto& contact : results) {
        // добавляем новую строку
        contactsTable->insertRow(row);
        
        // заполняем ячейки
        contactsTable->setItem(row, 0, new QTableWidgetItem(
            QString::fromStdString(contact.get_firstName())));
        contactsTable->setItem(row, 1, new QTableWidgetItem(
            QString::fromStdString(contact.get_lastName())));
        contactsTable->setItem(row, 2, new QTableWidgetItem(
            QString::fromStdString(contact.get_patronymic())));
        contactsTable->setItem(row, 3, new QTableWidgetItem(
            QString::fromStdString(contact.get_email())));
        contactsTable->setItem(row, 4, new QTableWidgetItem(
            QString::fromStdString(contact.get_birthDate())));
        
        // Новый столбец: телефоны
        QString phonesText = formatPhones(contact.get_phones());
        QTableWidgetItem *phonesItem = new QTableWidgetItem(phonesText);
        phonesItem->setToolTip(phonesText);
        contactsTable->setItem(row, 5, phonesItem);
        
        row++;
    }
    
    // включаем обратно сортировку
    contactsTable->setSortingEnabled(true);
    
    // подстраиваем ширину столбцов
    contactsTable->resizeColumnsToContents();
    
    // показываем количество найденных контактов
    if (results.empty()) {
        QMessageBox::information(this, "Search Results", 
            "No contacts found matching: \"" + query + "\"");
    }
}

/**
 * СЛОТ onClearForm
 * Обработчик очистки формы
 * Вызывается при нажатии кнопки "Clear Form"
 * Очищает все поля ввода
 */
void QtPhoneBook::onClearForm()
{
    firstNameEdit->clear();
    lastNameEdit->clear();
    patronymicEdit->clear();
    emailEdit->clear();
    birthDateEdit->clear();
    addressEdit->clear();
}
