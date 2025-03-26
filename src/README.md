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

- [ ] **[MVP]** Генерация случайного AES-256 ключа для шифрования системы
- [ ] **[MVP]** Рекурсивное вглубь шифрование файлов в указанной директории
- [ ] **[MVP]** Расшифровка указанной директории при предоставлении ключа
- [ ] Assign each attack a unique ID
- [ ] Send AES to C2 upon finishing encryption
- [ ] Exchange attack ID for public RSA from C2
- [ ] Encrypt the AES with RSA before sending
- [ ] Blacklist critical Windows files for encryption
- [ ] Reboot-proof the process

