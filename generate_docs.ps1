# Скрипт для автоматической генерации документации Doxygen
# Использование: .\generate_docs.ps1

param(
    [switch]$Open,
    [switch]$Watch
)

$env:Path = "$env:Path;C:\Program Files\doxygen\bin;C:\Program Files\Graphviz\bin"
$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$doxyfile = Join-Path $repoRoot "Doxyfile"
$indexFile = Join-Path $repoRoot "docs\doxygen\html\index.html"

Write-Host "📚 Генерация документации Doxygen..." -ForegroundColor Cyan
Push-Location $repoRoot

# Запуск Doxygen
doxygen $doxyfile

if ($?) {
    Write-Host "✅ Документация успешно сгенерирована!" -ForegroundColor Green
    Write-Host "📄 Путь: $indexFile" -ForegroundColor Yellow
    
    if ($Open) {
        Write-Host "🌐 Открываю в браузере..." -ForegroundColor Cyan
        Start-Process $indexFile
    }
}
else {
    Write-Host "❌ Ошибка при генерации документации" -ForegroundColor Red
    exit 1
}

Pop-Location
