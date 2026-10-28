# MathEquationSolver

Клиент-серверное приложение на C++17 / Qt для решения нелинейных уравнений
методом половинного деления и методом Ньютона. Требования — в `docs/Спецификация.docx`.

| Папка             | Что внутри                                                    |
|-------------------|---------------------------------------------------------------|
| `MathServer`      | TCP-сервер (порт 33333), вычисления, база SQLite `mathserver.db` |
| `MathClient`      | GUI-клиент: вход/регистрация, ввод параметров, результаты      |
| `MathServerTests` | Unit-тесты сервера (QTest)                                    |
| `MathClientTests` | Unit-тесты клиента (QTest)                                    |
| `docs/diagrams`   | Диаграммы классов сервера и клиента, диаграмма Use Case (PlantUML + PNG) |
| `docs/doxygen`    | Документация Doxygen (откройте `html/index.html`)              |
| `docs/GIT_STRUCTURE.md` | Ветки, правила коммитов, структура репозитория, схема `docs/git/branches.png` |
| `wiki`            | Исходники страниц [Wiki](https://github.com/SymenSoul/TechProg_RootFinder/wiki) |
| `Testing_MathSolver_Real.xlsx` | Тест-кейсы, карточки дефектов, итоговый отчёт     |
| `Test_Strategy.xlsx` | Тест-план (стратегия тестирования) и чек-лист      |

Ветки: `main` (стабильная), `develop` (интеграция), `feature/server`, `feature/client`,
`feature/tests`, `feature/docs`, `feature/docker` — подробнее в `docs/GIT_STRUCTURE.md`.

## Сборка и запуск

Нужен Qt 5.15 или 6.x. Проще всего открыть `.pro` файлы в Qt Creator. Из консоли:

```bash
mkdir build-server && cd build-server
qmake ../MathServer/MathServer.pro && make
./MathServer            # сервер создаст mathserver.db в текущей папке

mkdir build-client && cd build-client
qmake ../MathClient/MathClient.pro && make
./MathClient            # подключается к 127.0.0.1:33333
```

Сервер нужно запускать **до** клиента (см. дефект №10).

## Docker (сервер)

```bash
docker build -t mathserver .
docker run -d -p 33333:33333 -v mathserver-data:/data --name mathserver mathserver
# или
docker compose up -d
```

При сборке образа запускаются unit-тесты: если какой-то упадёт, образ не соберётся.
База данных хранится в томе `/data`. Клиент запускается на компьютере как обычно.

## Тесты

```bash
cd MathServerTests && qmake && make && ./MathServerTests
cd MathClientTests && qmake && make && ./MathClientTests
```

Подробнее, в том числе что означает `XFAIL`, — в `TESTING.md`.

## Документация Doxygen

```bash
doxygen Doxyfile        # нужен Graphviz для графов классов
```

Результат — `docs/doxygen/html/index.html`. Известные дефекты отмечены в коде тегом
`@bug` и собраны на странице «Ошибки».

## Протокол

| Запрос                         | Ответ                                    |
|--------------------------------|------------------------------------------|
| `reg <login> <password>`       | `SUCCESS` / `ALREADY_EXISTS`             |
| `auth <login> <password>`      | `SUCCESS` / `FAILED`                     |
| `solve <funcId> <a> <b> <eps>` | `бисекция\|Ньютон`, например `2.000122\|2.000000`; при неудаче вместо числа `FAILED` |
| `step <funcId> <x0> [<f'(x0)>]` | `x1 = x0 − f(x0)/f'(x0)`, например `2.166667`; `FAILED`, если f'(x0) = 0 |

Уравнения: `0` — x² − 4, `1` — sin(x) − x/2, `2` — eˣ − 2.
