# SOLID Priciples:
SOLID principles are a set of five guidelines in object-oriented programming that promote writing clean, maintainable, and scalable code. 

The SOLID principle helps in reducing tight coupling. Tight coupling means a group of classes are highly dependent on one another which you should avoid in your code.

Opposite of tight coupling is loose coupling and your code is considered as a good code when it has loosely-coupled classes.
Loosely coupled classes minimize changes in your code, helps in making code more reusable, maintainable, flexible and stable. Now let's discuss one by one these principles...


We'd like to make our code
1. Readable
2. Maintainable
3. Testable
4. Extensible
What you can't measure, you can't improve!

Also find helfull notes on SOLID Priciples:     [SOLID_Principles](SOLID_Principles.pdf)


## Single Responsibility Principle (SRP)

### Definition:
A class should have only one reason to change, meaning it should only have one job or responsibility.

### Problem:
When a class has multiple responsibilities, changes to one responsibility can affect others, making the code harder to maintain and test.

### Resolution:
if you see a piece of code that violates the SRP, then you should break it down into multiple pieces
Split responsibilities into separate classes. - we can use OOP tool "inhertance" to split this code.

### Problem Solved:
SRP prevents classes from becoming too complex and hard to maintain by separating concerns. It ensures that each class focuses on a single responsibility.

### Real-World Analogy:
A chef should cook, not also manage restaurant accounts. Similarly, a class should focus on one task, not multiple unrelated ones.

<div style="display: flex; justify-content: space-between; gap: 20px;">
<pre>
+-------------------+
|   Chef Class      |
|-------------------|
| Responsibility:   |
| Cooking           |
+-------------------+
</pre>

<div style="flex: 1; padding: 10px; border: 1px #ddd;">
<pre>
+-------------------+
|   Accountant Class|
|-------------------|
| Responsibility:   |
| Managing Accounts |
+-------------------+
</pre>
</div>
</div>

### Code Examples

### C++ Example:
```cpp

#include <iostream>
#include <string>

// Chef class - Responsible for cooking
class Chef {
public:
    void cook() {
        std::cout << "Chef is cooking food." << std::endl;
    }
};

// Accountant class - Responsible for managing accounts
class Accountant {
public:
    void manageAccounts() {
        std::cout << "Accountant is managing restaurant accounts." << std::endl;
    }
};

int main() {
    Chef chef;
    Accountant accountant;

    chef.cook();
    accountant.manageAccounts();

    return 0;
}
```
Another C++ Example:
```Cpp
#include <iostream>
#include <string>

class OrderManager {
public:
    void processOrder(const std::string& order) {
        std::cout << "Processing order: " << order << std::endl;
    }
};

class NotificationService {
public:
    void sendNotification(const std::string& message) {
        std::cout << "Sending notification: " << message << std::endl;
    }
};

int main() {
    OrderManager orderManager;
    NotificationService notificationService;

    orderManager.processOrder("Order #123");
    notificationService.sendNotification("Order #123 has been processed.");

    return 0;
}
```

### Java Example:
```Java
// Chef class - Responsible for cooking
class Chef {
    public void cook() {
        System.out.println("Chef is cooking food.");
    }
}

// Accountant class - Responsible for managing accounts
class Accountant {
    public void manageAccounts() {
        System.out.println("Accountant is managing restaurant accounts.");
    }
}

public class Main {
    public static void main(String[] args) {
        Chef chef = new Chef();
        Accountant accountant = new Accountant();

        chef.cook();
        accountant.manageAccounts();
    }
}
```
Another Java Example:
```Java
class OrderManager {
    public void processOrder(String order) {
        System.out.println("Processing order: " + order);
    }
}

class NotificationService {
    public void sendNotification(String message) {
        System.out.println("Sending notification: " + message);
    }
}

public class Main {
    public static void main(String[] args) {
        OrderManager orderManager = new OrderManager();
        NotificationService notificationService = new NotificationService();

        orderManager.processOrder("Order #123");
        notificationService.sendNotification("Order #123 has been processed.");
    }
}
```
---

## Open/Closed Principle (OCP)

### Definition:
Software entities (classes, modules, functions) should be open for extension but closed for modification.

### Problem:
Directly modifying existing code to add new functionality can introduce bugs and break existing behavior.

### Resolution:
Use abstraction (e.g., inheritance or interfaces) to extend functionality without modifying existing code.


### Problem Solved:
OCP ensures that existing code does not need to be modified when new functionality is added, reducing the risk of introducing bugs and maintaining backward compatibility.

### Real-World Analogy:
A plug-and-play socket allows you to add new devices without altering the socket itself.



<div style="display: flex; justify-content: space-between; gap: 20px;">
<pre>
+-------------------+
| Abstract Socket   |
|-------------------|
| Interface:        |
| ConnectDevice()   |
+-------------------+
</pre>
<div style="flex: 1; padding: 10px; border: 1px #ddd;">
<pre>
+-------------------+       +-------------------+
| Lamp Class        |       | Fan Class         |
|-------------------|       |-------------------|
| Implements Socket |       | Implements Socket |
+-------------------+       +-------------------+
</pre>
</div>
</div>


### Code Examples

### C++ Example:
```cpp

#include <iostream>
#include <string>

// Abstract class (interface)
class Device {
public:
    virtual void operate() = 0; // Pure virtual function
    virtual ~Device() {}
};

// Lamp class - Extends functionality
class Lamp : public Device {
public:
    void operate() override {
        std::cout << "Lamp is turned on." << std::endl;
    }
};

// Fan class - Extends functionality
class Fan : public Device {
public:
    void operate() override {
        std::cout << "Fan is spinning." << std::endl;
    }
};

void connectDevice(Device* device) {
    device->operate();
}

int main() {
    Lamp lamp;
    Fan fan;

    connectDevice(&lamp);
    connectDevice(&fan);

    return 0;
}
```
Another C++ Example:
```Cpp
#include <iostream>
#include <string>

class PaymentProcessor {
public:
    virtual void processPayment(double amount) = 0;
    virtual ~PaymentProcessor() {}
};

class CreditCardPayment : public PaymentProcessor {
public:
    void processPayment(double amount) override {
        std::cout << "Processing credit card payment of $" << amount << std::endl;
    }
};

class PayPalPayment : public PaymentProcessor {
public:
    void processPayment(double amount) override {
        std::cout << "Processing PayPal payment of $" << amount << std::endl;
    }
};

int main() {
    PaymentProcessor* payment = new CreditCardPayment();
    payment->processPayment(100.0);

    delete payment;
    payment = new PayPalPayment();
    payment->processPayment(200.0);

    delete payment;
    return 0;
}
```

### Java Example:
```Java
// Abstract class (interface)
interface Device {
    void operate();
}

// Lamp class - Extends functionality
class Lamp implements Device {
    public void operate() {
        System.out.println("Lamp is turned on.");
    }
}

// Fan class - Extends functionality
class Fan implements Device {
    public void operate() {
        System.out.println("Fan is spinning.");
    }
}

public class Main {
    public static void connectDevice(Device device) {
        device.operate();
    }

    public static void main(String[] args) {
        Device lamp = new Lamp();
        Device fan = new Fan();

        connectDevice(lamp);
        connectDevice(fan);
    }
}
```
Another Java Example:
```Java
interface PaymentProcessor {
    void processPayment(double amount);
}

class CreditCardPayment implements PaymentProcessor {
    public void processPayment(double amount) {
        System.out.println("Processing credit card payment of $" + amount);
    }
}

class PayPalPayment implements PaymentProcessor {
    public void processPayment(double amount) {
        System.out.println("Processing PayPal payment of $" + amount);
    }
}

public class Main {
    public static void main(String[] args) {
        PaymentProcessor payment = new CreditCardPayment();
        payment.processPayment(100.0);

        payment = new PayPalPayment();
        payment.processPayment(200.0);
    }
}
```

## Liskov Substitution Principle (LSP)

### Definition:
Objects of a superclass should be replaceable with objects of a subclass without affecting the correctness of the program.

### Problem:
Violating LSP leads to unexpected behavior when a subclass does not honor the contract of its superclass.

### Resolution:
Ensure subclasses behave consistently with the expectations set by the superclass.

### Problem Solved:
LSP ensures that derived classes behave consistently with their base classes, preventing unexpected behavior when substituting objects.

### Real-World Analogy:
A square should behave like a rectangle without breaking expectations. For example, if a rectangle's width and height can be set independently, a square should not violate this behavior.

<div style="display: flex; justify-content: space-between; gap: 20px;">
<pre>
+-------------------+
| Shape (Superclass)|
|-------------------|
| Area()            |
+-------------------+
</pre>
<div style="flex: 1; padding: 10px; border: 1px #ddd;">
<pre>
+-------------------+       +-------------------+
| Rectangle Class   |       | Square Class      |
|-------------------|       |-------------------|
| Overrides Area()  |       | Overrides Area()  |
+-------------------+       +-------------------+
</pre>
</div>
</div>

## Code Examples

### C++ Example
```cpp
#include <iostream>

class Rectangle {
public:
    virtual void setWidth(double width) { this->width = width; }
    virtual void setHeight(double height) { this->height = height; }
    virtual double getArea() const { return width * height; }

protected:
    double width, height;
};

class Square : public Rectangle {
public:
    void setWidth(double width) override {
        this->width = width;
        this->height = width; // Ensure square behavior
    }

    void setHeight(double height) override {
        this->height = height;
        this->width = height; // Ensure square behavior
    }
};

int main() {
    Rectangle* rect = new Square();
    rect->setWidth(5);
    rect->setHeight(5);

    std::cout << "Area: " << rect->getArea() << std::endl;

    delete rect;
    return 0;
}
```

### Java Example:
```Java
class Rectangle {
    protected double width, height;

    public void setWidth(double width) {
        this.width = width;
    }

    public void setHeight(double height) {
        this.height = height;
    }

    public double getArea() {
        return width * height;
    }
}

class Square extends Rectangle {
    @Override
    public void setWidth(double width) {
        this.width = width;
        this.height = width; // Ensure square behavior
    }

    @Override
    public void setHeight(double height) {
        this.height = height;
        this.width = height; // Ensure square behavior
    }
}

public class Main {
    public static void main(String[] args) {
        Rectangle rect = new Square();
        rect.setWidth(5);
        rect.setHeight(5);

        System.out.println("Area: " + rect.getArea());
    }
}
```


## Interface Segregation Principle (ISP)

### Definition:
A class should not be forced to implement interfaces it does not use. Instead, interfaces should be split into smaller, more specific ones.

### Problem:
La#rge interfaces force classes to implement methods they don't need, leading to unnecessary complexity.

### Resolution:
Create smaller, focused interfaces tailored to specific needs.

### Problem Solved:
ISP prevents classes from being burdened with unnecessary methods, making them easier to maintain and reducing coupling.

### Real-World Analogy:
A printer should not be required to implement methods for scanning if it only supports printing. Instead, separate interfaces for printing and scanning should be created.

<div style="display: flex; justify-content: space-between; gap: 20px;">
<pre>
+-------------------+
| Printable         |
|-------------------|
| print()           |
+-------------------+

+-------------------+
| Scannable         |
|-------------------|
| scan()            |
+-------------------+

+-------------------+       +-------------------+
| Printer Class     |       | Scanner Class     |
|-------------------|       |-------------------|
| Implements        |       | Implements        |
| Printable         |       | Scannable         |
+-------------------+       +-------------------+
</pre>
</div>

## Code Examples

### C++ Example:
```cpp
#include <iostream>

class Printer {
public:
    virtual void printDocument(const std::string& document) = 0;
    virtual ~Printer() {}
};

class Scanner {
public:
    virtual void scanDocument(const std::string& document) = 0;
    virtual ~Scanner() {}
};

class MultiFunctionDevice : public Printer, public Scanner {
public:
    void printDocument(const std::string& document) override {
        std::cout << "Printing: " << document << std::endl;
    }

    void scanDocument(const std::string& document) override {
        std::cout << "Scanning: " << document << std::endl;
    }
};

int main() {
    MultiFunctionDevice mfd;
    mfd.printDocument("Report.pdf");
    mfd.scanDocument("Photo.jpg");

    return 0;
}
```

### Java Example:
```Java
interface Printer {
    void printDocument(String document);
}

interface Scanner {
    void scanDocument(String document);
}

class MultiFunctionDevice implements Printer, Scanner {
    @Override
    public void printDocument(String document) {
        System.out.println("Printing: " + document);
    }

    @Override
    public void scanDocument(String document) {
        System.out.println("Scanning: " + document);
    }
}

public class Main {
    public static void main(String[] args) {
        MultiFunctionDevice mfd = new MultiFunctionDevice();
        mfd.printDocument("Report.pdf");
        mfd.scanDocument("Photo.jpg");
    }
}
```

## Dependency Inversion Principle (DIP)

### Definition:
High-level modules should depend on abstractions, not on low-level modules. Both should depend on abstractions.

### Problem:
Direct dependencies between high-level and low-level modules make the system rigid and hard to test.

### Resolution:
Introduce abstractions (e.g., interfaces) to decouple high-level and low-level modules.

### Problem Solved:
DIP decouples high-level and low-level modules, making the system more flexible and easier to maintain.

### Real-World Analogy:
A remote control works with any TV via an abstract interface, rather than being tied to a specific TV model.

<div style="display: flex; justify-content: space-between; gap: 20px;">
<pre>
+-------------------+
| Remote Interface  |
|-------------------|
| turnOn()          |
| turnOff()         |
+-------------------+

+-------------------+       +-------------------+
| SonyTV Class      |       | LGTV Class        |
|-------------------|       |-------------------|
| Implements Remote |       | Implements Remote |
+-------------------+       +-------------------+
</pre>
</div>

## Code Examples

### C++ Example
```cpp
#include <iostream>
#include <memory>

class TV {
public:
    virtual void turnOn() = 0;
    virtual void turnOff() = 0;
    virtual ~TV() {}
};

class SamsungTV : public TV {
public:
    void turnOn() override {
        std::cout << "Samsung TV is now ON." << std::endl;
    }

    void turnOff() override {
        std::cout << "Samsung TV is now OFF." << std::endl;
    }
};

class RemoteControl {
private:
    std::shared_ptr<TV> tv;
public:
    RemoteControl(std::shared_ptr<TV> tv) : tv(tv) {}

    void pressPowerButton() {
        static bool isOn = false;
        if (isOn) {
            tv->turnOff();
        } else {
            tv->turnOn();
        }
        isOn = !isOn;
    }
};

int main() {
    std::shared_ptr<TV> samsungTV = std::make_shared<SamsungTV>();
    RemoteControl remote(samsungTV);

    remote.pressPowerButton();
    remote.pressPowerButton();

    return 0;
}
```

### Java Example:
```Java
interface TV {
    void turnOn();
    void turnOff();
}

class SamsungTV implements TV {
    @Override
    public void turnOn() {
        System.out.println("Samsung TV is now ON.");
    }

    @Override
    public void turnOff() {
        System.out.println("Samsung TV is now OFF.");
    }
}

class RemoteControl {
    private TV tv;

    public RemoteControl(TV tv) {
        this.tv = tv;
    }

    public void pressPowerButton() {
        tv.turnOn();
        tv.turnOff();
    }
}

public class Main {
    public static void main(String[] args) {
        TV samsungTV = new SamsungTV();
        RemoteControl remote = new RemoteControl(samsungTV);

        remote.pressPowerButton();
    }
}
```