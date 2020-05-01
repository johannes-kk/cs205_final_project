#include "datasets.hpp"
#include <assert.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <vector>


/*
 * DATA VECTOR - ACCESSORS :
 */


int DataVector::size() const
{
    /** Returns the number of values in the row/column. */
    return this->size_;
}

bool DataVector::is_row() const
{
    /**
     * Returns true if the vector represents a row
     * and false if it represents a column.
     */
    return this->is_row_;
}

bool DataVector::is_locked() const
{
    /** Checks if object is read-only. */
    return this->is_locked_;
}

double DataVector::value(int i) const
{
    /** Get cell in given position (positive or negative index). */
    if (i>=0)
    {
        // Index from beginning (positive):
        assert ( i<this->size() );
    } else {
        // Index from end (negative):
        assert ( i>=-this->size() );
        i += this->size();
    }
    return this->values_[ i ];
}

std::vector<double> DataVector::vector() const
{
    /** Get a copy of the values as a vector of doubles. */
    std::vector<double> vector;
    for (int i = 0; i < this->size(); i++)
    {
        vector.push_back( this->values_[ i ] );
    }
    return vector;
}


/*
 * DATA VECTOR - UTILITES :
 */


void DataVector::lock()
{
    /** Prevent unwanted changes to data. */
    this->is_locked_ = true;
}

void DataVector::addValue(double value)
{
    /** Adds a cell to a row. */
    //assert (!this->is_locked());
    this->values_.push_back(value);
    this->size_ += 1;
}



std::string DataVector::to_string(bool new_line, int col_width) const
{
    std::string out = "";
    for (int i = 0; i < this->size(); i++)
    {
        out += "| ";
        double value = this->value(i);
        std::string pad,val = "";
        val = std::to_string(value);
        // Add space to align negative sign:
        if (value>=0)
        {
            val = ' '+val;
        }
        // Truncate if too long:
        if (val.length()>col_width)
        {
            val = val.substr(0,col_width);
        }
        // Pad if too short:
        if (val.length()<col_width)
        {
            pad.append(col_width-val.length(),' ');
        }
        // Append padding and value:
        out += pad;
        out += val;
        if ((!this->is_row()) or (i==this->size()-1))
        {
            out += " |\n";
        } else {
            out += ' ';
        }
    }
    // Add (optional) extra newline character:
    if (new_line)
    {
        out += '\n';
    }
    return out;
}   

void DataVector::print(bool new_line, int col_width) const
{
    std::cout << this->to_string(new_line, col_width);
}


/*
 * DATA VECTOR - CONSTRUCTORS :
 */


DataVector::DataVector(bool is_row)
{
    this->is_row_ = is_row;
    this->is_locked_ = false;
    this->size_ = 0;
}

DataVector::DataVector(std::vector<double> vector, bool is_row)
{
    this->is_row_ = is_row;
    this->is_locked_ = false;
    this->size_ = 0;
    for (int i = 0; i < vector.size(); i++)
    {
        this->addValue( vector[i] );
    }
    //this->lock();
}



/*
 * DATA VECTOR - I/O :
 */


std::ostream& operator<<(std::ostream& os, const DataVector& datavector)
{
    os << datavector.to_string(false);  // Don't add extra newline character.
    return os;
}


/*
 * DATA FRAME - ACCESSORS :
 */


int DataFrame::length() const
{
    /** Returns the number of rows in the frame. */
    return this->length_;
}

int DataFrame::width() const
{
    /** Returns the number of columns in the frame. */
    return this->width_;
}

bool DataFrame::is_locked() const
{
    /** Checks if object is read-only. */
    return this->is_locked_;
}

DataVector* DataFrame::row(int r) const
{
    /** Get pointer to given row (stored internally). */
    if (r>=0)
    {
        // Index from beginning (positive):
        assert ( r<this->length() );
    } else {
        // Index from end (negative):
        assert ( r>=-this->length() );
        r += this->length();
    }
    return this->rows_[ r ];
}

DataVector* DataFrame::col(int c) const
{
    /** Get given column (constructed on the fly). */
    DataVector* col = new DataVector(false);  // is_row==false.
    if (c>=0)
    {
        // Index from beginning (positive):
        assert ( c<this->width() );
    } else {
        // Index from end (negative):
        assert ( c>=-this->width() );
        c += this->width();
    }
    for (int i = 0; i < this->length(); i++)
    {
        col->addValue( this->rows_[i]->value(c) );
    }
    //column.lock();
    return col;
}

double DataFrame::value(int r, int c) const
{
    /** Get value in given row and column. */
    return this->row(r)->value(c);
}

std::vector<std::vector<double>> DataFrame::matrix() const
{
    /** Get a copy of values as a vector of vectors of doubles. */
    std::vector<std::vector<double>> matrix;
    for (int i = 0; i < this->length(); i++)
    {
        std::vector<double> temp_vector;
        for (int j = 0; j < this->length(); j++)
        {
            temp_vector.push_back( this->value(i,j) );
        }
        matrix.push_back(temp_vector);
    }
    return matrix;
}


/*
 * DATA FRAME - UTILITES :
 */


void DataFrame::lock()
{
    /** Prevent unwanted changes to data. */
    this->is_locked_ = true;
    for (int i = 0; i < this->length(); i++)
    {
        this->rows_[i]->lock();
    }
}

void DataFrame::addRow(DataVector *row)
{
    /** Append the pointer to the list of rows. */
    //assert (!this->is_locked());
    assert (row->is_row());
    assert (row->size()==this->width());
    this->rows_.push_back(row);
}

void DataFrame::addRow(std::vector<double> vector)
{
    /** Wrap the values in a DataRow and add its pointer to the list. */
    //assert (!this->is_locked());
    assert (!vector.size()==this->width());
    DataVector* row = new DataVector(true);  // is_row==true.
    for (int i = 0; i < this->width(); i++)
    {
        row->addValue( vector[i] );
    }
    //row.lock();
    this->rows_.push_back(row);
}

void DataFrame::addCol(DataVector *col)
{
    /** Append the values to each row in the list. */
    //assert (!this->is_locked());
    assert (!col->is_row());
    assert (col->size()==this->length());
    for (int i = 0; i < this->length(); i++)
    {
        assert (!this->rows_[i]->is_locked());
        this->rows_[i]->addValue( col->value(i) );
    }
}

void DataFrame::addCol(std::vector<double> vector)
{
    /** Append the values to each row in the list. */
    //assert (!this->is_locked());
    assert (vector.size()==this->length());
    for (int i = 0; i < this->length(); i++)
    {
        assert (!this->rows_[i]->is_locked());
        this->rows_[i]->addValue( vector[i] );
    }
}

std::string DataFrame::to_string(bool new_line, int col_width) const
{
    std::string out = "";
    for (int i = 0; i < this->length(); i++)
    {
        out += this->row(i)->to_string(false, col_width);  // new_line==false.
    }
    // Add (optional) extra newline character:
    if (new_line)
    {
        out += '\n';
    }
    return out;
}   

void DataFrame::print(bool new_line, int col_width) const
{
    std::cout << this->to_string(new_line, col_width);
}


/*
 * DATA FRAME - CONSTRUCTORS :
 */


DataFrame::DataFrame()
{
    this->is_locked_ = false;
    this->width_ = 0;
    this->length_ = 0;
}

DataFrame::DataFrame(std::vector<std::vector<double>> matrix)
{
    this->is_locked_ = false;
    this->length_ = matrix.size();
    if (matrix.size()>0){
        this->width_ = matrix[0].size();
    } else {
        this->width_ = 0;
    }
    for (int i = 0; i < this->length_; i++)
    {
        assert (matrix[i].size()==this->width());
        DataVector* row = new DataVector(matrix[i],true);  // is_row==true.
        //row->lock();
        this->addRow(row);
    }
    //this->lock();
}


/*
 * DATA FRAME - I/O :
 */


std::ostream& operator<<(std::ostream& os, const DataFrame& dataframe)
{
    os << dataframe.to_string(false);  // Don't add extra newline character.
    return os;
}


/*
 * DATA LOADER - ACCESSORS :
 */


/*
 * DATA LOADER - UTILITES :
 */

DataFrame DataLoader::load()
{
    /** Returns the loaded frame. */
    return this->dataframe_;
}


/*
 * DATA LOADER - CONSTRUCTORS :
 */


DataLoader::DataLoader()
{
    /** Load hard-coded dummy dataset. */
    std::vector<std::vector<double>> matrix = {
        {2.7810836,2.550537003,0}, 
        {1.465489372,2.362125076,0},
        {7.627531214,2.759262235,1}, 
        {3.396561688,4.400293529,0},
        {8.675418651,-0.242068655,1}, 
        {1.38807019,1.850220317,0}, 
        {3.06407232,3.005305973,0},  
        {5.332441248,2.088626775,1}, 
        {6.922596716,1.77106367,1},  
        {7.673756466,3.508563011,1},
        {7.673756466,3.301233593,1}
    };
    this->dataframe_ = DataFrame(matrix);
}

DataLoader::DataLoader(std::vector<std::vector<double>> matrix)
{
    /** Load dataset from vector of vectors. */
    this->dataframe_ = DataFrame(matrix);
}
