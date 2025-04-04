# Encryption

>Роль, ответственная за процесс шифрования.

Я не придумал параграф с описанием. 


## Содержание

1. [Дисклеймер](#дисклеймер)
2. [Использование](#использование)
3. [TO-DO](#to-do)


## Дисклеймер

Запуская приведенный здесь исходный код на своей машине, вы отдаете себе отчет в том (и берете на себя ответственность за то), что он может привести к необратимым последствиям.


## Использование

Из корня репозитория:

```
git switch encryption; cd src/; make
```

Это сбилдит и запустит безопасную демонстрацию (работает исключительно в пределах директории `dummy/`). 


## TO-DO

- [x] **[MVP]** Генерация случайного ключа для шифрования
- [x] **[MVP]** Рекурсивный обход указанного пути и шифрование встречаемых файлов с использованием AES-256
- [x] **[MVP]** Расшифровка файлов по указанному пути
- [ ] Safeguard the functionality
- [ ] Handle keys in Base64
- [ ] Assign each attack a unique ID
- [ ] Send AES to C2 upon finishing encryption
- [ ] Exchange attack ID for public RSA from C2
- [ ] Encrypt the AES with RSA before sending
- [ ] Blacklist critical Windows files for encryption
- [ ] Reboot-proof the process

