#pragma once

#include <stdexcept>
#include <iostream>
#include <cstdio>

class Matrix final
{
private:
    class row;

public:
    explicit Matrix(std::size_t rows, std::size_t columns) : columns_(columns)
                                                           , rows_(rows)
    {
        data = new int*[rows];
        for(std::size_t i = 0; i < rows; i++) data[i] = new int[columns];
    }
    explicit Matrix(std::size_t rows, std::size_t columns, int** m) : columns_(columns)
                                                                    , rows_(rows)
    {
        data = new int*[rows];
        for(std::size_t i = 0; i < rows; i++) data[i] = new int[columns];
        for(std::size_t i = 0; i < rows_; i++)
            for(std::size_t j = 0; j < columns_; j++) data[i][j] = m[i][j];
    }
    Matrix(const Matrix& m) : columns_(m.columns_)
                            , rows_(m.rows_)
    {
        data = new int*[m.rows_];
        for(std::size_t i = 0; i < m.rows_; i++) data[i] = new int[m.columns_];
        for(std::size_t i = 0; i < m.rows_; i++)
            for(std::size_t j = 0; j < m.columns_; j++) data[i][j] = m.data[i][j];
    }
    ~Matrix();
    std::size_t getRows() const;
    std::size_t getColumns() const;
    Matrix& operator*=(const int rvalue);
    const bool operator==(const Matrix& rvalue) const;
    const bool operator!=(const Matrix& rvalue) const;
    friend std::ostream& operator<<(std::ostream& out, const Matrix& matrix);

    row operator[](unsigned int index)
    {
        if(index >= this->rows_) throw std::out_of_range("Invalid first (row) index");
        return row(columns_, data[index]);
    }
    const row operator[](unsigned int index) const
    {
        if(index >= this->rows_) throw std::out_of_range("Invalid first (row) index");
        return row(columns_, data[index]);
    }

private:

    class row
    {
    public:
        explicit row(std::size_t columns, int* row) : columns_r(columns), data(row) {}
        int& operator[](unsigned int index) 
        { 
            if(index >= columns_r) throw std::out_of_range("Invalid second (column) index");
            return this->data[index]; 
        }
        const int& operator[](unsigned int index) const 
        { 
            if(index >= columns_r) throw std::out_of_range("Invalid second (column) index");
            return this->data[index]; 
        }
    private:
        std::size_t columns_r;
        int* data;
    };

    std::size_t columns_;
    std::size_t rows_;
    int** data;
};

std::ostream& operator<<(std::ostream& out, const Matrix& matrix);