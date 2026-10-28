# Структура Git

Используется упрощённая модель Git Flow: стабильная ветка `main`, ветка интеграции `develop`
и отдельная ветка под каждое направление работы.

![Схема веток](https://raw.githubusercontent.com/SymenSoul/TechProg_RootFinder/main/docs/git/branches.png)

| Ветка            | Назначение |
|------------------|------------|
| `main`           | Стабильная версия для сдачи. Только слияния из `develop`, версии отмечаются тегами `v1.0`, `v1.1`… |
| `develop`        | Интеграция готовых изменений |
| `feature/server` | Сервер: протокол, вычисления, база данных |
| `feature/client` | Клиент: формы, подключение к серверу |
| `feature/tests`  | Unit-тесты и тест-кейсы |
| `feature/docs`   | Doxygen, диаграммы, Wiki |
| `feature/docker` | Docker |

Для исправления дефекта — ветка `bugfix/defect-<номер>` от `develop` (например, `bugfix/defect-02`).

## Правила

1. В `main` напрямую не коммитим.
2. Работа идёт в `feature/*`, слияние в `develop` — через Pull Request.
3. Перед слиянием в `main` все проекты собираются и unit-тесты проходят.
4. Сообщение коммита: `Тип: что сделано`, где тип — `Add`, `Fix`, `Docs`, `Test` или `Refactor`.
   Пример: `Fix: дефект №2, solve только после входа`.

## Что не хранится в репозитории

Сгенерированные при сборке файлы (`moc_*.cpp`, `ui_*.h`, `Makefile`, `*.o`), папки сборки,
личные настройки Qt Creator (`*.pro.user`) и база `mathserver.db` — см. `.gitignore`.

Полное описание со структурой папок и командами —
[`docs/GIT_STRUCTURE.md`](https://github.com/SymenSoul/TechProg_RootFinder/blob/main/docs/GIT_STRUCTURE.md).
