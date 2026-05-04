-----------------------------------------------------------------------------------------
Benchmark                                               Time             CPU   Iterations
-----------------------------------------------------------------------------------------
RealImageFixture/Variant1_Single                 12470632 ns     12276786 ns           56
RealImageFixture/Variant2a_Interleaved/1        191028114 ns        0.000 ns          100
RealImageFixture/Variant2a_Interleaved/2        183877176 ns        0.000 ns          100
RealImageFixture/Variant2a_Interleaved/4        179147067 ns        0.000 ns          100
RealImageFixture/Variant2a_Interleaved/8        169106280 ns       312500 ns          100
RealImageFixture/Variant2a_Interleaved/16       140510878 ns       468750 ns          100
RealImageFixture/Variant2b_Blocked/1            181804084 ns       156250 ns          100
RealImageFixture/Variant2b_Blocked/2            137395527 ns       156250 ns          100
RealImageFixture/Variant2b_Blocked/4            143802415 ns        0.000 ns          100
RealImageFixture/Variant2b_Blocked/8            147814234 ns       312500 ns          100
RealImageFixture/Variant2b_Blocked/16           139054679 ns        0.000 ns          100
RealImageFixture/Variant3_Local/1                12324184 ns        15625 ns         1000
RealImageFixture/Variant3_Local/2                 6191784 ns        46875 ns         1000
RealImageFixture/Variant3_Local/4                 3679137 ns        31250 ns         1000
RealImageFixture/Variant3_Local/8                 2719376 ns       156250 ns         1000
RealImageFixture/Variant3_Local/16                2182336 ns       251116 ns         2240
SyntheticImageFixture/Variant1_Single            29002516 ns     28125000 ns           25
SyntheticImageFixture/Variant2a_Interleaved/1   189178980 ns        0.000 ns          100
SyntheticImageFixture/Variant2a_Interleaved/2   438225230 ns        0.000 ns           10
SyntheticImageFixture/Variant2a_Interleaved/4   530576090 ns        0.000 ns           10
SyntheticImageFixture/Variant2a_Interleaved/8   570073670 ns        0.000 ns           10
SyntheticImageFixture/Variant2a_Interleaved/16  664703520 ns        0.000 ns           10
SyntheticImageFixture/Variant2b_Blocked/1       189742151 ns        0.000 ns          100
SyntheticImageFixture/Variant2b_Blocked/2       692714030 ns        0.000 ns           10
SyntheticImageFixture/Variant2b_Blocked/4       481645360 ns        0.000 ns           10
SyntheticImageFixture/Variant2b_Blocked/8       542851730 ns        0.000 ns           10
SyntheticImageFixture/Variant2b_Blocked/16      551410070 ns        0.000 ns           10
SyntheticImageFixture/Variant3_Local/1           26217427 ns        0.000 ns          100
SyntheticImageFixture/Variant3_Local/2           12942408 ns        15625 ns         1000
SyntheticImageFixture/Variant3_Local/4            7389216 ns        93750 ns         1000
SyntheticImageFixture/Variant3_Local/8            4700879 ns       140625 ns         1000
SyntheticImageFixture/Variant3_Local/16           3563921 ns       279018 ns          896
HugeImageFixture/Variant1_Single                128550567 ns    130208333 ns            6
HugeImageFixture/Variant2a_Interleaved/1        817207160 ns        0.000 ns           10
HugeImageFixture/Variant2a_Interleaved/2       2043304520 ns        0.000 ns           10
HugeImageFixture/Variant2a_Interleaved/4       2212454840 ns        0.000 ns           10
HugeImageFixture/Variant2a_Interleaved/8       2321324950 ns        0.000 ns           10
HugeImageFixture/Variant2a_Interleaved/16      2801184900 ns        0.000 ns            1
HugeImageFixture/Variant2b_Blocked/1            806458690 ns        0.000 ns           10
HugeImageFixture/Variant2b_Blocked/2           2986229700 ns        0.000 ns            1
HugeImageFixture/Variant2b_Blocked/4           2205769380 ns        0.000 ns           10
HugeImageFixture/Variant2b_Blocked/8           2291519440 ns        0.000 ns           10
HugeImageFixture/Variant2b_Blocked/16          2359950310 ns        0.000 ns           10
HugeImageFixture/Variant3_Local/1               111366224 ns       156250 ns          100
HugeImageFixture/Variant3_Local/2                57086293 ns        0.000 ns          100
HugeImageFixture/Variant3_Local/4                31249451 ns        0.000 ns          100
HugeImageFixture/Variant3_Local/8                17670793 ns       171875 ns         1000
HugeImageFixture/Variant3_Local/16               11083858 ns       203125 ns         1000

Отличные результаты! Ваши бенчмарки отработали идеально, и полученные данные дают исчерпывающую картину того, как процессор работает с памятью и потоками. 

Основываясь на ваших данных, я подготовил итоговый аналитический отчёт (Этап 4).

---

## ОТЧЁТ О ПРОИЗВОДИТЕЛЬНОСТИ: АЛГОРИТМЫ ПОСТРОЕНИЯ ГИСТОГРАММЫ

### 1. Проверка корректности
Все алгоритмы доказали свою математическую корректность на Этапе 1 с использованием GoogleTest.
* Однопоточный и параллельные алгоритмы (`Interleaved`, `Blocked`, `Local`) выдают идентичные значения.
* Сумма элементов нормализованной гистограммы по каждому каналу строго равна **1.0**.

---

### 2. Результаты измерений (Сводка)
Для наглядности выделим ключевые показатели времени выполнения (в миллисекундах) на 1 и 16 потоках.

| Алгоритм | Real Image (Mixed) | Synthetic Image (FFFFFF) | Huge Image (8000x8000) |
| :--- | :--- | :--- | :--- |
| **1_Single** | 12.5 ms | 29.0 ms | 128.5 ms |
| **2a_Interleaved (1t / 16t)** | 191 ms / 140 ms | 189 ms / 664 ms | 817 ms / 2801 ms |
| **2b_Blocked (1t / 16t)** | 181 ms / 139 ms | 189 ms / 551 ms | 806 ms / 2359 ms |
| **3_Local (1t / 16t)** | 12.3 ms / **2.2 ms** | 26.2 ms / **3.5 ms** | 111.3 ms / **11.0 ms** |

---

### 3. Анализ результатов

**Однопоточный vs Параллельные решения**
Параллелизм не всегда означает ускорение. Использование атомарных операций (`Variant2a` и `Variant2b`) привело к **катастрофическому замедлению** программы (более чем в 10 раз по сравнению с однопоточным решением на старте). При этом `Variant3_Local` показал отличную масштабируемость, ускорив вычисления в 5-11 раз на 16 потоках.

**Атомарные схемы: Interleaved vs Blocked и проблема кэша**
Атомарные подходы масштабируются отрицательно. Чем больше потоков мы добавляем, тем медленнее работает код (например, на `HugeImage` время выросло с 817 мс до 2801 мс). 
* **Причина:** Истинное и ложное разделение (True/False Sharing). Каждый вызов `fetch_add` заставляет процессор блокировать строку кэша (Cache Line) и синхронизировать её между ядрами по шине данных (Cache Coherency Ping-Pong). 
* **Blocked быстрее Interleaved:** В схеме `Interleaved` счетчики R, G и B для одного цвета лежат рядом в памяти `[R, G, B, R, G, B]`. Когда один поток обновляет Red, а другой Green для одного и того же оттенка, они дерутся за одну кэш-линию. В схеме `Blocked` массивы `[R...R], [G...G], [B...B]` разнесены, что снижает вероятность False Sharing.

**Почему локальные гистограммы (Local) выигрывают?**
Это решение использует принцип **Shared Nothing**. Каждый поток работает только со своим локальным массивом, который целиком помещается в L1-кэш ядра. Межъядерная синхронизация отсутствует полностью на протяжении всего цикла обработки пикселей. Синхронизация (суммирование) происходит лишь один раз в самом конце. Как итог — почти линейное ускорение (на `HugeImage` время упало со 111 мс до 11 мс).

**Влияние содержимого изображения**
Реалистичное фото (`mojave.jpg`) и синтетика (`FFFFFF`) показывают кардинально разную производительность на атомиках:
* В реальном фото цвета распределены (пиксели попадают в разные индексы от 0 до 255), поэтому потоки реже сталкиваются в одной строке кэша. Атомарное решение смогло немного ускориться на 16 потоках (с 191 мс до 140 мс).
* В синтетике абсолютно все пиксели белые. Все 16 потоков безостановочно бомбардируют один и тот же адрес в памяти (`r[255], g[255], b[255]`). Возникает колоссальная очередь (Contention), и время выполнения улетает в космос (664 мс).

---

### 4. Итоговые выводы

1. **Самый быстрый подход:** Использование локальных частичных гистограмм (`Variant3_Local`). Идеально утилизирует многоядерные архитектуры и L1/L2 кэши.
2. **Самый простой подход:** Однопоточный (`Variant1`). Без затрат на инициализацию потоков он отлично справляется с небольшими изображениями.
3. **Роль атомарных операций:** В задачах интенсивного агрегирования данных (где количество корзин мало, а обращений много) `std::atomic` становится главным узким местом из-за оверхеда на протоколы когерентности кэша (MESI). Их применение здесь нецелесообразно.
4. **Организация памяти:** Плотная упаковка данных (`Interleaved`) вредит производительности при конкурентном доступе из-за эффекта ложного разделения. Разделение данных (`Blocked`) или изоляция контекстов потоков (`Local`) — критически важные паттерны для написания быстрого C++ кода.