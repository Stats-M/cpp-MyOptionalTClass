#include <stdexcept>
#include <utility>
#include <cstring>

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

    Optional(const T& value) : is_initialized_(true)
    {
        // ������ � �������� ��������� �� ������ � ������
        //value_ = new (&data_[0]) T(value);
        // ������ ��� �������� ��������� �� ������ � ������
        new (&data_[0]) T(value);
    }
    Optional(T&& value) : is_initialized_(true)
    {
        // ������ � �������� ��������� �� ������ � ������
        //value_ = new (&data_[0]) T(std::move(value));
        // ������ ��� �������� ��������� �� ������ � ������
        new (&data_[0]) T(std::move(value));
    }
    Optional(const Optional& other)
    {
        if (other.is_initialized_)
        {
            new (&data_[0]) T(other.Value());
            is_initialized_ = true;
        }
    }
    Optional(Optional&& other)
    {
        if (other.is_initialized_)
        {
            new (&data_[0]) T(std::move(other.Value()));
            is_initialized_ = true;
        }
    }

    Optional& operator=(const T& value);
    Optional& operator=(T&& rhs)
    {
        data_
    }
    Optional& operator=(const Optional& rhs)
    {
        // �������� �����
        std::strcpy(data_, rhs.data_);
        // ���� rhs ����� ���� �������������, ������� ������
        if (rhs.is_initialized_)
        {
            value_ = &(data_);
        }

        return *this;
    }
    Optional& operator=(Optional&& rhs);

    ~Optional()
    {
        // ������� ������, ���� ����
        if (is_initialized_)
        {
            value_->~T();
            // �� ��������� ���, ������ ���������
            //is_initialized_ = false;
        }
        // ����� �� �����. ������� �� ���������
        //operator delete (data_);
    }

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
        if (!this->HasValue())
        {
            throw BadOptionalAccess();
        }

        return *value_;
    }

    void Reset()
    {
        // �������� ���������� ������������ � ���������� ������ �������
        value_->~T();
        is_initialized_ = false;

        // ����� �� �������, Reset == �������� ������� � ����������
        // ������ � ������ ����������
        // ����� �� �����, ������� �� ���������.
        //operator delete (data_);
    }

private:
    // ����� ��� �������� 1 ������� ���� T.
    // alignas ����� ��� ����������� ������������ ����� ������
    // ����� �� �����, operator new/delete �� ���������
    alignas(T) char data_[sizeof(T)];
    // ��������� �� ����������� ��� ������ operator new � ������ ������ ���� �
    T* value_ = nullptr;

    bool is_initialized_ = false;
};