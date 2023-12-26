# Интеграция ImageProcessor
Для того, чтобы использовать библиотеку, были выполнены некоторые действия:
- В проект была добавлена папка libs для того, чтобы добавлять туда компоненты poromarker
- В poromarker.cpp добавлен заголовочный файл Imageprocessor/ImageProcessor.hpp
- Добавлена структура ImageProcessor::Settings config для хранения настроек сегментации.
- При помощи imfilebrowser и функции ImageProcessor::setConfigFilePath(path) была реализована возможность <br/> выбора папки для хранения config - файла.
- Реализована инициализация настроек:
    - При отсутствии файла конфигурации вызывается ImageProcessor::createDefaultConfig()
    - Загружаются текущие настройки с помощью ImageProcessor::readConfig() 
- Обработка изменений настроек в UI была реализована посредством:
    - Вызова ImageProcessor::updateConfig(config) при изменении параметров config
    - Сохранения измененной конфигурации
- Выполнение сегментации изображения реализовано с помощью:
    - ImageProcessor::createMasks(images, config)
