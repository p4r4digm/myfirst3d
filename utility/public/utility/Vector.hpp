#pragma once

#include "MemoryBuffer.hpp"
#include "Assert.hpp"
#include <initializer_list>

namespace utl {
   template<typename T>
   class Vector {
      MemoryBuffer m_buffer;
      size_t m_size, m_capacity;

      void grow() {
         if (!m_buffer) {
            m_buffer = std::move(MemoryBuffer{});
         }

         //vector was empty
         if (!m_capacity) {
            m_capacity = 8;
            m_buffer.setSize(m_capacity * sizeof(T));
            return;
         }

         //create the new buffer and set its head
         MemoryBuffer newBuffer;
         newBuffer.setSize(m_capacity * sizeof(T) * 2);
         T *newBegin = (T*)newBuffer.data();

         //move our shit over there
         for (auto && obj : *this) {
            new(newBegin++) T(std::move(obj));
         }

         //clear ourselves
         auto size = m_size;
         clear();

         //move back
         m_size = size;
         m_capacity *= 2;
         m_buffer = std::move(newBuffer);
      }

   public:
      Vector() :m_size(0), m_capacity(0) {
      }
      Vector(std::initializer_list<T> l) :m_size(0), m_capacity(0) {
         for (auto && item : l) {
            push_back(item);
         }
      }
      Vector(size_t size) :m_size(0), m_capacity(0) {         
         while (size--) {
            push_back(T{});
         }
      }
      Vector(MemoryBuffer && buffer)
         :m_buffer(std::move(buffer)) {
         m_size = m_buffer.getSize() / sizeof(T);
         m_capacity = m_buffer.getCapacity() / sizeof(T);
      }
      ~Vector() {
         clear();
      }

      Vector(Vector<T> const &other) {
         m_size = m_capacity = 0;
         for (auto && o : other) {
            push_back(o);
         }
      }

      Vector<T> &operator=(Vector<T> const &other) {
         if (this == &other) {
            return *this;
         }

         clear();
         for (auto &&o : other) {
            push_back(o);
         }
         return *this;
      }

      Vector(Vector<T> &&other){
         m_capacity = other.m_capacity;
         m_size = other.m_size;
         m_buffer = std::move(other.getBuffer());
      }

      Vector<T> &operator=(Vector<T> &&other) {
         if (this == &other) {
            return *this;
         }

         clear();         
         m_capacity = other.m_capacity;
         m_size = other.m_size;
         m_buffer = std::move(other.getBuffer());
         return *this;
      }

      T &operator[](size_t i) {
         if (i >= size()) { return *(T*)0; }

         return *(begin() + i);
      }

      T &front() {
         ASSERT(m_size, "Vector is empty!");
         
         return operator[](0);
      }

      T &back() {
         ASSERT(m_size, "Vector is empty!");

         return operator[](m_size - 1);
      }

      T const *begin() const {
         return (T*)m_buffer.data();
      }

      T const *end() const {
         return begin() + size();
      }


      T *begin() {
         return (T*)m_buffer.data();
      }

      T *end() {
         return begin() + size();
      }

      

      MemoryBuffer &&getBuffer() {
         m_size = 0;
         m_capacity = 0;
         return std::move(m_buffer);
      }

      void push_back(T const &obj) {
         if (m_size == m_capacity) {
            grow();
         }

         //construct obj at the correct place in our buffer
         new(begin() + (m_size++)) T(obj);
         m_buffer.setSize(m_size * sizeof(T));
      }

      void push_back(T && obj) {
         if (m_size == m_capacity) {
            grow();
         }

         //construct obj at the correct place in our buffer         
         new(begin() + (m_size++)) T(std::move(obj));
         m_buffer.setSize(m_size * sizeof(T));
         
      }

      void clear() {
         if (m_buffer) {
            while (m_size > 0) {
               begin()[--m_size].~T();
            }

            m_buffer.clear();
         }
         
         m_size = m_capacity = 0;
      }

      size_t size() const {
         return m_size;
      }

   };
}