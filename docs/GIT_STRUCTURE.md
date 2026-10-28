# Структура Git-репозитория MathEquationSolver

Репозиторий: <https://github.com/SymenSoul/TechProg_RootFinder>

## 1. Ветки

Используется упрощённая модель Git Flow: стабильная ветка `main`, ветка
интеграции `develop` и отдельная ветка под каждое направление работы.

![Схема веток](git/branches.png)

*Схема показывает порядок работы с ветками. Исходник картинки — `docs/git/branches.mmd` (Mermaid).*

| Ветка             | Назначение                                                                 | Откуда создаётся | Куда сливается |
|-------------------|----------------------------------------------------------------------------|------------------|----------------|
| `main`            | Стабильная версия, которая сдаётся и разворачивается. Напрямую не коммитим | —                | —              |
| `develop`         | Интеграция: сюда сливаются готовые изменения из `feature/*`                 | `main`           | `main`         |
| `feature/server`  | Сервер `MathServer`: протокол, вычисления, база данных                      | `develop`        | `develop`      |
| `feature/client`  | Клиент `MathClient`: формы, сетевое подключение                             | `develop`        | `develop`      |
| `feature/tests`   | Unit-тесты (`MathServerTests`, `MathClientTests`) и тест-кейсы в xlsx       | `develop`        | `develop`      |
| `feature/docs`    | Doxygen, UML-диаграммы, Wiki, README                                        | `develop`        | `develop`      |
| `feature/docker`  | `Dockerfile`, `docker-compose.yml`, `.dockerignore`                         | `develop`        | `develop`      |

Для исправления конкретного дефекта из `Testing_MathSolver_Real.xlsx` можно
создавать короткую ветку `bugfix/defect-<номер>` от `develop`, например
`bugfix/defect-02` («solve без авторизации»).

### Правила

1. В `main` попадает только проверенный код из `develop`. Перед слиянием должны
   собираться все четыре проекта и проходить unit-тесты (`docker build` делает это автоматически).
2. Каждый участник работает в своей ветке `feature/*` и регулярно подтягивает
   изменения из `develop`, чтобы не копить конфликты.
3. Слияние в `develop` — через Pull Request на GitHub, чтобы второй участник мог посмотреть изменения.
4. Версии, которые сдаются, отмечаются тегом на `main`: `v1.0`, `v1.1`, …

## 2. Сообщения коммитов

Формат: `Тип: что сделано`. Этот формат уже используется в истории (`Add: all files of repo`).

| Тип        | Когда                         | Пример                                        |
|------------|-------------------------------|-----------------------------------------------|
| `Add`      | новый файл или функция        | `Add: тесты протокола CommandParsing`         |
| `Fix`      | исправление дефекта           | `Fix: дефект №2, solve только после входа`    |
| `Docs`     | документация, диаграммы, Wiki | `Docs: диаграмма Use Case`                    |
| `Test`     | тесты и тест-кейсы            | `Test: TC-07 интервал a > b`                  |
| `Refactor` | изменения без новой функции   | `Refactor: вынести проверку eps в MathSolver` |

## 3. Структура файлов

```
TechProg_RootFinder/
├── MathServer/              сервер: main.cpp, mytcpserver, parsing, database, mathsolver
├── MathClient/              клиент: authregform, mainform (.ui), clientcontroller, mytcpclient
├── MathServerTests/         unit-тесты сервера (QTest)
├── MathClientTests/         unit-тесты клиента (QTest)
├── docs/
│   ├── Спецификация.docx    требования к системе
│   ├── diagrams/            диаграммы классов и Use Case (.puml + .png)
│   ├── doxygen/html/        документация Doxygen (index.html)
│   ├── git/                 схема веток (.mmd + .png)
│   ├── mainpage.md          главная страница Doxygen
│   └── GIT_STRUCTURE.md     этот файл
├── wiki/                    исходники страниц GitHub Wiki
├── Testing_MathSolver_Real.xlsx   тест-кейсы, дефекты, отчёт
├── Dockerfile, docker-compose.yml, .dockerignore
├── Doxyfile
├── README.md, TESTING.md
└── .gitignore
```

## 4. Что не хранится в Git (`.gitignore`)

| Шаблон                                 | Почему                                                         |
|----------------------------------------|----------------------------------------------------------------|
| `moc_*.cpp`, `ui_*.h`, `*.moc`, `*.o`, `Makefile` | генерируются qmake/moc/uic при каждой сборке          |
| `build-*/`, `*.pro.user*`              | папки сборки и личные настройки Qt Creator                     |
| `*.db`                                 | `mathserver.db` сервер создаёт сам; в базе пароли пользователей |

## 5. Как создавались ветки

```bash
git checkout main
git pull
git branch develop
git branch feature/server
git branch feature/client
git branch feature/tests
git branch feature/docs
git branch feature/docker
git push -u origin develop feature/server feature/client feature/tests feature/docs feature/docker
```

Проверка: `git branch -a` — ветки должны быть видны и локально, и как `remotes/origin/...`.

Типичная работа в ветке:

```bash
git checkout feature/server
git merge develop                 # подтянуть свежие изменения
# ... правки ...
git add MathServer/parsing.cpp
git commit -m "Fix: дефект №2, solve только после входа"
git push
# затем на GitHub: Pull Request feature/server -> develop
```
