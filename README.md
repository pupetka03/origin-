<p align="center">
  <img src="logo.png" alt="Origin++ Logo" width="200px" style="border-radius: 20px;"/>
</p>

# Origin++

**Origin++** is a high-level programming language that is compiled together into highly optimized C++17.

**Origin++** — це високорівнева мова програмування, яка транспілюється безпосередньо у високооптимізований C++17.

---

## 🚀 Features / Особливості

* **No Semicolons** / Без крапок з комою — clean Python-like code structure.
* **Auto-typing** / Автовизначення типів — variables do not require explicit type declarations.
* **Tuple Unpacking & Multi-return** / Розпаковка кортежів — return multiple values (`return a, b`) and unpack them (`x, y = fun()`).
* **Advanced Signatures** / Робота з типами — declare list/array return types (`int[]`) and union variant types (`int abo string[]`).
* **Ukrainian Boolean Logic** / Українська булева логіка — `ta` (AND), `abo` (OR), `ne` (NOT).
* **Slavic Branching** / Слов'янське розгалуження — `jak` (if), `inakjak` (else if), `inak` (else).
* **Protected Identifiers** / Захист від колізій — names starting with `_` are forbidden to protect transpiler keywords.
* **Blazing Fast** / Блискавична швидкість — transpiles directly to C++17 with `-O3` optimizations.
* **One-step Run** / Запуск в один крок — automated build and execution script.

---

## 📝 Syntax Reference / Довідник з синтаксису

### 1. Variables & Printing / Змінні та Вивід
```python
x = 10 //int
y = "Hello, World!" // const char* x = "text";
z = string(y) // string

# Print outputs variables, strings, lists, and tuples separated by commas / Вивід через кому
print("x is: ", x, " and y is: ", y)
print(z)
```

### 2. Boolean Logic & Branching / Логіка та Розгалуження
```python
i = 0
y = 2

# Using 'ta' (and), 'abo' (or), 'ne' (not)
jak (i == 0) ta (y == 2) {
    print("Logical AND!")
}

jak i == 9 {
    print("9")
} inakjak i == 0 {
    print("0")
} inak {
    print("Other value")
}
```

### 3. Loops & Loop Controls / Цикли та керування
```python
# 'cykl' is a while loop / Цикл while
i = 0
cykl i < 5 {
    i = i + 1
    jak i == 2 {
        continue # Skip / Пропустити
    }
    jak i == 4 {
        break    # Exit loop / Вийти з циклу
    }
    print(i)
}

# 'takt' is a for loop with 'range' / Цикл for з інтервалом
takt i range(1, 4) {
    print(i)
}

# 'nic' is a placeholder (like pass in Python) / Заглушка nic
jak i == 5 {
    nic
}
```

### 4. Functions & Return / Функції та Повернення
```python
# 1. Simple function / Проста функція
baza main() {
    return 5
}

sum = main()
print(sum)

# 2. Multiple return values & unpacking / Багатозначне повернення та розпаковка
baza get_user() {
    age = 21
    name = "Ihor"
    return age, name
}

user_age, user_name = get_user()
print("User: ", user_name, " Age: ", user_age)

# 3. List and variant signature types / Сигнатури списків та варіантів (abo)
baza get_data(x) -> int[] abo int {
    jak x == 0 {
        return 0
    } inak {
        arr -> int = [1, 2, 3]
        return arr
    }
}

# 4. Nested lists inside tuple return types / Вкладені списки всередині кортежів
baza get_user_data(x) -> int abo [string[], int] {
    jak x == 1 {
        return 1
    } inak {
        d -> string = ["John", "Doe"]
        return d, 25
    }
}
```

### 5. Lists & Arrays / Списки та Масиви
```python
# Creating typed lists / Створення типізованих списків
numbers -> int = [1, 2, 3]
names -> string = ["John", "Mary"]

# Empty list declaration / Оголошення порожнього списку
empty_list -> int = []

# Accessing elements by index / Доступ до елементів за індексом
first = numbers[0]

# Appending elements / Додавання елементів
numbers.append(4)

# Getting list size / Отримання розміру списку
size = numbers.size()

# Iterating over list / Перебір списку
takt x numbers {
    print(x)
}
```

### 6. Type Conversions / Приведення типів
```python
x = int(5.9)      # 5
y = int("45")     # 45
z = string(100)   # "100"
```

### 7. Dictionaries / Словники (`dict`)
Origin++ supports a robust dictionary type mapping keys to values using the clean shorthand syntax `KeyType:ValueType`:

```python
# 1. Creating and initializing dictionaries / Створення та ініціалізація словників
prices -> string:double = {{"яблуко", 25.5}, {"банан", 40.0}}
empty_dict -> int:string = {}

# 2. Adding & updating elements / Додавання та оновлення
prices["апельсин"] = 65.0
prices["яблуко"] = 28.0

# 3. Checking key existence / Перевірка наявності ключа
jak prices.has_key("вишня") {
    print("Ціна вишні: ", prices["вишня"])
} inak {
    print("Вишні немає на складі")
}

# 4. Deleting elements / Видалення елементів
prices.erase("банан")   # Removes key 'банан'
prices.clear()          # Clears whole dictionary

# 5. Helper properties / Корисні властивості
count = prices.size()   # Number of elements
is_empty = prices.empty() # Check if empty (true/false)

# 6. Iterating over dictionary / Обхід словника у циклі
takt item prices {
    print("Товар: ", item.first, " Ціна: ", item.second)
}
```

### 8. File Operations / Робота з файлами
File handling is supported via `f_open` with modes `"r"` (read), `"w"` (write), or `"a"` (append):

```python
# 1. Writing to a file / Запис у файл
f_out = f_open("output.txt", "w")
jak f_out.otkryty() {
    f_out.zapis("Hello from Origin++!")
    f_out.novy_riadok()
    f_out.zakryty()
}

# 2. Reading a file line-by-line / Порядкове читання файлу
f_in = f_open("output.txt", "r")
jak f_in.otkryty() {
    cykl true {
        line = f_in.citat_riadok()
        jak line == "" {
            break
        }
        print(line)
    }
    f_in.zakryty()
}

# 3. Read helper methods / Інші методи читання
# f_in.citat()          # Reads entire file as a single string (preserves newlines)
# f_in.citat_riadky()   # Reads all lines into a list[string]
```

### 9. OOP & Classes / Об'єктно-орієнтоване програмування
Origin++ features clean, native object-oriented capabilities (classes, constructors, inheritance, and methods):

```python
# 1. Defining classes and inheritance / Оголошення класів та наслідування
typ People() {
    x -> string
}

typ Developer(People) {
    xx -> int = 5

    # Constructor named after the class / Конструктор
    baza Developer(x, xx) {
        nas.x = x      # 'nas' acts as 'this' / 'nas' працює як 'this'
        nas.xx = xx
    }

    # Class method / Метод класу (automatically virtual if no parameters)
    baza info() {
        print("Розробник: ", nas.x, " Рівень: ", nas.xx)
    }
}

# 2. Object Instantiation / Створення об'єкта
dev = Developer("Origin++ Creator", 10)

# 3. Member and method access / Звернення до полів та методів об'єкта
print("Стаж: ", dev.xx)
dev.info()
```

### 10. Variant Operations & Indexing / Робота з Variant-типами та Індексація

Origin++ надає потужні вбудовані засоби для прозорої роботи з типом `Variant` (об'єднаннями `abo`):

#### 1. Автоматична розпаковка варіантів (Variant Destructuring)
Якщо функція повертає `Variant` (наприклад, `int abo [string[], int]`), ви можете розпакувати її напряму. Транслятор автоматично згенерує `std::get` на основі кількості змінних:
```python
res, res_age = get_user_data(2)  # Автоматично витягне та розпакує кортеж
```

#### 2. Індексація об'єднань (Variant Subscript)
Якщо `Variant` містить у собі списки, ви можете звертатися до їхніх елементів за індексом безпосередньо. Результатом буде новий `Variant` з типами елементів:
```python
res = get_data(1)    # повертає Variant із списком
first_item = res[0]  # працює! first_item буде типу Variant
```

#### 3. Рантайм-безпека при додаванні у списки
Метод `append` у списках тепер автоматично приймає `Variant`. Якщо під час виконання тип активного значення сумісний із типом списку, воно додається, інакше виникає помилка виконання.
```python
rrr -> int = [10]
res = get_user_data(1) # повертає Variant (який зараз містить int)
rrr.append(res)        # працює автоматично!
```

#### 4. Конвертація Variant-типів
Вбудовані функції приведення типів `int(val)`, `double(val)`, `string(val)` повністю підтримують `Variant`. Вони безпечно витягують поточне значення або повертають помилку, якщо тип несумісний.
```python
res = get_user_data(1)
print(int(res) + 1) # працює!
```


---

## 🛠️ Getting Started / Як запустити

Origin++ includes an automated shell script that compiles the transpiler, generates C++ code, compiles it, and runs the binary.

Origin++ має вбудований скрипт, який автоматично компілює транслятор, перекладає твою програму в C++, компілює її та запускає готовий бінарник.

```bash
# Clone the repository / Склонуйте репозиторій
git clone <repository-url>
cd origin++

# Run your script (e.g. or.txt) / Запустіть скрипт
./run.sh or.txt
```
