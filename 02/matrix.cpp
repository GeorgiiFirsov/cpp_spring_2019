#include "matrix.h"

Matrix::~Matrix()
{
    for(std::size_t i = 0; i < rows_; i++) delete[] data[i];
    delete[] data;
}

std::size_t Matrix::getRows() const
{
    return this->rows_;
}

std::size_t Matrix::getColumns() const
{
    return this->columns_;
}

Matrix& Matrix::operator*=(const int rvalue)
{
    for(std::size_t i = 0; i < this->rows_; i++)
        for(std::size_t j = 0; j < this->columns_; j++) data[i][j] *= rvalue;
    return *this;
}

const bool Matrix::operator==(const Matrix& rvalue) const
{
    if(this->rows_ != rvalue.rows_ || this->columns_ != rvalue.columns_) return false;
    for(std::size_t i = 0; i < this->rows_; i++)
        for(std::size_t j = 0; j < this->columns_; j++)
            if(this->data[i][j] != rvalue.data[i][j]) return false;
    return true;
}

const bool Matrix::operator!=(const Matrix& rvalue) const
{
    return !(*this == rvalue);
}

std::ostream& operator<<(std::ostream& out, const Matrix& matrix)
{
    for(std::size_t i = 0; i < matrix.rows_; i++)
    {
        for(std::size_t j = 0; j < matrix.columns_; j++)
            out << matrix.data[i][j] << " ";
        out << std::endl;
    }
    return out;
}
