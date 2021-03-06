# Интерпретатор scheme

Реализация интерпретатора для
lisp-подобного языка программирования, а именно некоторого подмножества scheme. 

Язык состоит из:
 - Примитивных типов: целых чисел, bool-ов и _символов_ (идентификаторов).
 - Составных типов: пар и списков.
 - Переменных с синтаксической областью видимости.
 - Функций и лямбда-выражений.

Программа выполняет выражения языка и возвращает результат выполнения.

## Выполнение выражений
Выполнение языка происходит в 3 этапа:

1. **Токенизация** - преобразует текст программы в последовательность
   атомарных лексем. 

2. **Синтаксический анализ** - преобразует последовательность токенов
   в дерево операций.
   
3. **Вычисление** - рекурсивно обходит дерево программы и преобразует его
   в соответствии с набором правил.

## 1. Токенизация
Разбиение выражения на последовательность токенов:

- **Число:** `42`, `-4` или `+10`
- **Boolean** - либо `#t`, либо `#f`
- **Скобка:** `(` или `)`
- **Quote:** `'`
- **Dot:** `.`
- **Symbol:** Начинается с символов `[a-zA-Z<=>*/#]` и может содержать внутри символы `[a-zA-Z<=>*/#0-9?!-]`. Отдельные
  знаки `+` и `-` мы тоже считаем символами.

## 2. Синтаксический анализ

## Синтаксис scheme

* Выражение может быть парой. Пара обозначается круглыми скобками с точкой между двумя элементами. Например, для выражения `(1 . 2)` строится дерево из 3-х элементов. Корнем дерева
  является пара с двумя потомками.


* Выражение может быть списком. Список это одна из двух сущностей:

   1. Пустой список. Обозначается пустыми скобочками - ().
   2. Пара, в которой второй элемент это список. В этом случае первый элемент пары называется головой (head) списка, а
      второй - хвостом (tail).

Из этого определения следует, что список из трех элементов можно записать так:

```
(1 . (2 . (3 . ())))

 pair => pair => pair => ()
  |       |       |
  1       2       3
```

Однако так записывать списки неудобно и можно использовать сокращенную запись

```
(1 2 3)
```

Элементы списков могут любыми корректными выражениями

```
((1 . 2) (3 4) 5)
(1 () (2 3 4) 5)
```


## 3. Вычисление

### 1. Особая форма quote

`quote` - операция, которая при
вычислении просто возвращает свой аргумент. Специальный токен `'` - это синтаксический сахар
для `quote`. То есть, `'1` = `(quote 1)`, `(1 2)` = `(quote (1 2))` и так далее.

### 2. Integer

Целочисленный тип `int64_t`, предполагается, что переполнения не происходит.

Операции:

- NumberPred ("number?")
- Equal ("=")
- Less ("<")
- Greater (">")
- LessEqual ("<=")
- GreaterEqual (">=")
- Addition ("+")
- Subtraction ("-")
- Multiplication ("*")
- Division ("/")
- Min ("min")
- Max ("max")
- Abs ("abs")


### 3. Boolean

Имеет два возможных значения: `#t` и `#f`.

Операции:
- BooleanPred ("boolean?")
- Not ("not")
- And ("and")
- Or ("or")

### 4. List

Операции:
- PairPred ("pair?")
- NullPred ("null?")
- Cons ("cons")
- Car ("car")
- Cdr ("cdr")
- List ("list")
- ListRef ("list-ref")
- ListTail ("list-tail")

### 5. If

Возможны 2 формы записи.

* `(if condition true-branch)`
* `(if condition true-branch false-branch)`

Сначала вычисляет `condition` и проверяет значение на истинность. Затем вычисляет либо `true-branch`, либо `false-branch` и возвращает как результат
всего `if`-а.

### 6. Переменные

Поддержка переменных реализована с помощью особых форм `define` и `set!`.

```scheme
$ (define x 1)
> ()
$ x
> 1
```

Команда `set!` используется для изменения значения **существующей**
переменной:

```scheme
$ (define x 1)
> ()
$ (set! x 2)
$ x
> 2
$ (set1 y 1)
> NameError
```

Еще есть особые формы `set-car!` и `set-cdr!` - устанавливают первый и второй элементы в паре соответственно.

### Лямбда-функции

Синтаксис:

* `(lambda (x) (+ 1 x))`
* `(lambda (x y) (* y x))`
* `(lambda (x) (set! x (* x 2)) (+ 1 x))`
* `(lambda () 1)`

Создаёт новую функцию. Сначала перечисляется список аргументов функции, затем её тело. Тело может состоять из нескольких
выражений, в этом случае они вычисляются по порядку а результат последнего выражения становится результатом функции.

### Захват контекста

#### Lambda capture in C++

Также возможен и захват контекста. Синтаксис примерно совпадает с C++:

```c++
auto Range(int x) {
  return [&x] () {
    ++x;
    return x;
  };
}

void F() {
  auto r = Range(10);

  std::cout << r() << std::endl; // 11
  std::cout << r() << std::endl; // 12
}
```

В **Scheme**:

```scheme
$ (define range
    (lambda (x)
      (lambda ()
        (set! x (+ x 1))
        x)))

$ (define my-range (range 10))

$ (my-range)
> 11

$ (my-range)
> 12
```

### Работа с памятью

Возможен сценарий, когда два объекта ссылаются друг на друга ("циклические
ссылки"). `std::shared_ptr` в таком случае не обеспечивает корректного удаления.

Для работы с памятью существует сборщик мусора, учитывающий циклические ссылки.

### Обработка ошибок

Интерпретатор различает 3 вида ошибок:

1. `SyntaxError`: ошибки синтаксиса. Возникают, когда программа не соответствует формальному синтаксису языка. Или когда
   программа неправильно использует особые формы.


2. `NameError`: ошибки обращения к неопределённым переменным.


3. `RuntimeError`: ошибки времени исполнения. К этим ошибкам относятся все остальные ошибки которые могу возникнуть во
   время выполнения программы. Например: неправильное количество аргументов передано в функцию, неправильный тип
   аргумента.
