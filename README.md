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
* **Ukrainian Boolean Logic** / Українська булева логіка — `ta` (AND), `abo` (OR), `ne` (NOT).
* **Slavic Branching** / Слов'янське розгалуження — `jak` (if), `inakjak` (else if), `inak` (else).
* **Blazing Fast** / Блискавична швидкість — transpiles directly to C++17 with `-O3` optimizations.
* **One-step Run** / Запуск в один крок — automated build and execution script.

---

## 📝 Syntax Reference / Довідник з синтаксису

### 1. Variables & Printing / Змінні та Вивід
```python
x = 10 //int
y = "Hello, World!" // const char* x = "text";
z = string(y) // string

# Print automatically outputs variables and strings / Автоматичний вивід
print("x is: " x)
print(y)
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
# Functions are defined with 'baza' / Визначення функції
baza main() {
    return 5
}

sum = main()
print(sum)
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
