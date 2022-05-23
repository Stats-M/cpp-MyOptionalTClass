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

    bool HasValue() const
    {
        return is_initialized_;
    }

    // ��������� * � -> �� ������ ������ ������� �������� �� ������� Optional.
    // ��� �������� �������� �� ������� ������������
    T& operator*()
    {
        return *this->value_;
    }

    const T& operator*() const
    {
        return *this->value_;
    }

    T* operator->()
    {
        return this->value_;
    }

    const T* operator->() const
    {
        return this->value_;
    }

    // ����� Value() ���������� ���������� BadOptionalAccess, ���� Optional ����
    T& Value()
    {
        if (!this->HasValue())
        {
            throw BadOptionalAccess();
        }

        return *value_;
    }

    const T& Value() const
    {
        return *value_;
    }

    void Reset()
    {
        // ������� �������� ���������� ������������ � ����������
        // ������ �������
        value_->~T();
        // ����� ������� �����
        operator delete (data_);
    }

private:
    // ����� ��� �������� 1 ������� ���� T
    // alignas ����� ��� ����������� ������������ ����� ������
    alignas(T) char data_[sizeof(T)];
    // ��������� �� ����������� ��� ������ operator new � ������ ������ ���� �
    T* value_ = nullptr;

    bool is_initialized_ = false;
};