#include <stdexcept>
#include <utility>

// ���������� ����� ���� ������ ������������� ��� ��������� � ������� optional
class BadOptionalAccess : public std::exception
{
public:
    using exception::exception;

    virtual const char* what() const noexcept override
    {
        return "Bad optional access";
    }
};

template <typename T>
class Optional
{
public:
    Optional() = default;
    Optional(const T& value);
    Optional(T&& value);
    Optional(const Optional& other);
    Optional(Optional&& other);

    Optional& operator=(const T& value);
    Optional& operator=(T&& rhs);
    Optional& operator=(const Optional& rhs);
    Optional& operator=(Optional&& rhs);

    ~Optional();

    bool HasValue() const;

    // ��������� * � -> �� ������ ������ ������� �������� �� ������� Optional.
    // ��� �������� �������� �� ������� ������������
    T& operator*();
    const T& operator*() const;
    T* operator->();
    const T* operator->() const;

    // ����� Value() ���������� ���������� BadOptionalAccess, ���� Optional ����
    T& Value();
    const T& Value() const;

    void Reset();

private:
    // alignas ����� ��� ����������� ������������ ����� ������
    alignas(T) char data_[sizeof(T)];
    bool is_initialized_ = false;
};