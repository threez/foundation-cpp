/*
 *  test.h
 *  foundation-cpp
 *
 *  Created by Vincent Landgraf on 16.11.10.
 *  Copyright 2010 Vincent Landgraf. All rights reserved.
 *
 */
#ifndef FOUNDATION_TEST
#define FOUNDATION_TEST

#include <iostream>
#include <sstream>

namespace Foundation {
  namespace Test {
    template<typename T>
    class AssertionException : public std::exception {
    public:
      const char * file;
      const char * function;
      const char * expectationText;
      const char * valueText;
      const int line;
      const T expectation;
      const T value;
      
      AssertionException(const char * expectationText, T expectation, 
                         const char * valueText, T value, 
                         const char * file, 
                         const char * function, 
                         const int line)
      :expectationText(expectationText), expectation(expectation), 
       valueText(valueText), value(value), 
       file(file), function(function), line(line)
      {}
      
      const char* what() const throw() {
        std::ostringstream error;
        error << this->file << ":" << this->line
        << " " << this->function << "(): expected '" 
        << this->expectation << "' but got '" << this->value << "'";
        return error.str().c_str();
      }
    };
    
    template<typename T>
    void EqualAssertion(T expectation, T value, 
                        const char * file, const char * function, const int line) {
      if (expectation != value) {
        throw AssertionException<T>("expected", expectation, 
                                    "but got", value, 
                                    file, function, line);
      }
    }
    
    template<typename T>
    void NotEqualAssertion(T expectation, T value, 
                           const char * file, const char * function, const int line) {
      if (expectation == value) {
        throw AssertionException<T>("expected not", expectation, 
                                    "but got", value, 
                                    file, function, line);
      }
    }
    typedef void (*testFunction)();
    
    class Suite {
    private:
    
      const char * name;
      int tests, failed, passed, suiteSize, maxSuiteSize;
      testFunction *suiteItems;
      std::ostringstream errors;
    
    public:
      
      Suite(const char * name, const int size = 10)
      :name(name), tests(0), failed(0), passed(0), suiteSize(0), 
       maxSuiteSize(size), suiteItems(new testFunction[size])
      {}
      
      /*
       * add the passed test function to the suite
       */
      void operator<<(testFunction fn) {
        if (this->suiteSize >= this->maxSuiteSize) {
          throw "Error: size of test suite is exceeded!";
        }
        this->suiteItems[this->suiteSize++] = fn;
      }
      
      /*
       * run all tests of the suite
       */
      void run() {
        std::cout << "Running unit tests for " << name << std::endl;
        for (int i = 0; i < this->suiteSize; ++i) {
          this->test(this->suiteItems[i]);
        }
        this->finished();
      }
    
    protected:
      
      /*
       * run the passed test function
       */
      void test(testFunction fn) {
        try {
          this->tests++;
          fn();
          this->passed++;
          std::cout << ".";
        }
        catch (std::exception& exception) {
          this->failed++;
          std::cout << "F";
          errors << std::endl << " - " << exception.what() << std::endl << std::endl;
        }
        catch (char * exception) {
          this->failed++;
          std::cout << "F";
          errors << std::endl << " - " << exception << std::endl << std::endl;
        }
      }
      
      /*
       * display results after running the test
       */
      void finished() {
        std::cout << std::endl;
        
        if (this->failed > 0) {
          std::cout << "Test FAILED (ok: " << this->passed << 
          ", failed: " << this->failed << " of " << this->tests << ")" 
          << std::endl
          << this->errors.str(); 
        } else {
          std::cout << "Test OK (" << this->passed << 
          " of " << this->tests << ")" << std::endl; 
        }
      }
    };
  };
};

#define assertEquals(x, y) Test::EqualAssertion((x), (y), __FILE__, __FUNCTION__, __LINE__)
#define assertNotEquals(x, y) Test::NotEqualAssertion((x), (y), __FILE__, __FUNCTION__, __LINE__)
#define assertThrows(exception, cause) \
try { \
(cause); \
Test::EqualAssertion("throwing of exception", "nothing to catch", __FILE__, __FUNCTION__, __LINE__); \
} \
catch (exception ex) {}

#endif
