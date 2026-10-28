═══════════════════════════════════════════════════════════════════════════════
КРАТКАЯ ИНСТРУКЦИЯ ПО ЗАПУСКУ ТЕСТОВ
═══════════════════════════════════════════════════════════════════════════════

1. ПОДГОТОВКА ОКРУЖЕНИЯ

   Убедитесь, что в PATH добавлены:
   - C:\Qt\6.11.1\mingw_64\bin
   - C:\Qt\Tools\mingw1310_64\bin

   PowerShell команда:
   $env:PATH = "C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.11.1\mingw_64\bin;$env:PATH"

───────────────────────────────────────────────────────────────────────────────
2. ЗАПУСК СЕРВЕРНЫХ ТЕСТОВ
───────────────────────────────────────────────────────────────────────────────

   Путь: REPOSITORY_dlya_git\MathServerTests\debug\MathServerTests.exe

   Базовый запуск:
   .\MathServerTests.exe

   С выводом в файл:
   .\MathServerTests.exe -txt -o results.txt

   Список всех тестов:
   .\MathServerTests.exe -functions

   Запуск конкретного теста:
   .\MathServerTests.exe testRegisterNewUser

   Ожидаемый результат:
   ✓ 59 тестов PASS
   ✓ 5 тестов XFAIL (ожидаемые дефекты)

───────────────────────────────────────────────────────────────────────────────
3. ЗАПУСК КЛИЕНТСКИХ ТЕСТОВ
───────────────────────────────────────────────────────────────────────────────

   Путь: REPOSITORY_dlya_git\MathClientTests\debug\MathClientTests.exe

   Базовый запуск:
   .\MathClientTests.exe

   С выводом в файл:
   .\MathClientTests.exe -txt -o results.txt

   Список всех тестов:
   .\MathClientTests.exe -functions

   Ожидаемый результат:
   ✓ 21 тест PASS

───────────────────────────────────────────────────────────────────────────────
4. ИНТЕГРАЦИОННОЕ ТЕСТИРОВАНИЕ
───────────────────────────────────────────────────────────────────────────────

   4.1. Запустите сервер:
        cd REPOSITORY_dlya_git\MathServer\debug
        .\MathServer.exe

   4.2. Дождитесь сообщения "Сервер слушает порт 33333"

   4.3. В новом окне запустите клиент:
        cd REPOSITORY_dlya_git\MathClient\build\Desktop_Qt_6_11_1_MinGW_64_bit\debug
        .\MathClient.exe

   4.4. Проверьте функционал:
        - Регистрация нового пользователя
        - Вход под созданным пользователем
        - Решение уравнения (выбрать функцию, задать a, b, ε, нажать "Вычислить")

───────────────────────────────────────────────────────────────────────────────
5. ПЕРЕСБОРКА ПРОЕКТОВ
───────────────────────────────────────────────────────────────────────────────

   5.1. MathServer:
        cd REPOSITORY_dlya_git\MathServer
        qmake MathServer.pro -spec win32-g++ "CONFIG+=debug"
        mingw32-make -f Makefile.Debug

   5.2. MathClient:
        cd REPOSITORY_dlya_git\MathClient
        qmake MathClient.pro -spec win32-g++ "CONFIG+=debug"
        mingw32-make -f Makefile.Debug

   5.3. MathServerTests:
        cd REPOSITORY_dlya_git\MathServerTests
        qmake MathServerTests.pro -spec win32-g++ "CONFIG+=debug"
        mingw32-make -f Makefile.Debug

   5.4. MathClientTests:
        cd REPOSITORY_dlya_git\MathClientTests
        qmake MathClientTests.pro -spec win32-g++ "CONFIG+=debug"
        mingw32-make -f Makefile.Debug

───────────────────────────────────────────────────────────────────────────────
6. ВАЖНЫЕ ЗАМЕТКИ
───────────────────────────────────────────────────────────────────────────────

   • Qt Test по умолчанию не выводит результаты без флага -txt/-xml
   • Exit code 0 означает успешное прохождение всех тестов
   • XFAIL (Expected Fail) — ожидаемые дефекты, не считаются ошибкой
   • Сервер создаёт mathserver.db в папке с исполняемым файлом
   • Для работы нужны все DLL из списка в TEST_RESULTS.txt

───────────────────────────────────────────────────────────────────────────────
7. РЕШЕНИЕ ПРОБЛЕМ
───────────────────────────────────────────────────────────────────────────────

   Ошибка "не удаётся запустить программу":
   → Скопируйте все DLL из C:\Qt\6.11.1\mingw_64\bin и 
     C:\Qt\Tools\mingw1310_64\bin в папку с .exe

   Ошибка "QSQLITE driver not loaded":
   → Создайте папку sqldrivers\ рядом с .exe
   → Скопируйте туда qsqlite.dll из C:\Qt\6.11.1\mingw_64\plugins\sqldrivers\

   Ошибка "This application failed to start":
   → Создайте папку platforms\ рядом с .exe
   → Скопируйте туда qwindows.dll из C:\Qt\6.11.1\mingw_64\plugins\platforms\

   Сервер не принимает подключения:
   → Проверьте, что порт 33333 не занят другим процессом
   → Проверьте firewall/антивирус

   Тесты не выводят результаты:
   → Используйте флаг -txt или -o output.txt

═══════════════════════════════════════════════════════════════════════════════
