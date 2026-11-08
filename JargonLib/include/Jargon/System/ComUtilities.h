#pragma once

#include "Jargon/ScopedPointer.h"
#include "Jargon/System/WindowsDefines.h"

#include <windows.h>

struct IUnknown;


#define JARGON_IF_HR_SUCCESS(fn) \
	do { \
		if (SUCCEEDED(hr)) { \
			hr = (fn); \
		} \
	} while (0)

#define JARGON_RETURN_FALSE_IF_HR_FAIL(fn) \
	do { \
		hr = (fn); \
		if (FAILED(hr)) {\
			return false; \
		}\
	} while (0)


namespace Jargon{
namespace System{

	class ComInitializer {
		public:
			ComInitializer(DWORD flags);

			~ComInitializer();

			operator bool() const;
		private:
			bool initialized = false;
	};

	// A release policy for IUnknown subtypes that need to be freed with IUnknown::Release();
	class ComPtrReleasePolicy {
		public:
			static void Release(IUnknown* toRelease);
	};

	template<class ComClass>
	class ComPtr : public Jargon::ScopedPointer<ComClass, ComPtrReleasePolicy> {
		public:
			~ComPtr() {
			}

			ComPtr<ComClass>& operator=(ComClass* other) {
				swapTarget(other);
				return *this;
			}

			ComPtr<ComClass>& operator=(ComPtr<ComClass>& other) {
				swapTarget(other.releaseOwnership());
				return *this;
			}
	};

	// A release policy for types that need to be freed with CoTaskMemFree()
	class CoTaskMemReleasePolicy {
		public:
			static void Release(void* toRelease);
	};

	// Similar to ScopedPointer, but the template parameter is the pointer type, not the
	// type pointed to. This is necessary because a lot of COM pointer types have additional
	// qualifiers like __unaligned, so it is easier to just define a pointer like:
	//      CoTaskMemPtr<PIDLIST_ABSOLUTE> myPointer;
	template<class ComPtrType, class ReleasePolicyClass = CoTaskMemReleasePolicy>
	class CoTaskMemPtr {
		public:
			CoTaskMemPtr(){
				m_target = nullptr;
			}

			~CoTaskMemPtr(){
				releaseTarget();
			}

			ComPtrType getRaw() const{
				return m_target;
			}

			ComPtrType* addressOf() {
				return &m_target;
			}

			bool isNull() const{
				return m_target == nullptr;
			}

			operator bool() const {
				return !isNull();
			}

			template<class T>
			void set(T * other){
				this->operator=(other);
			}

			void swap(CoTaskMemPtr<ComPtrType, ReleasePolicyClass> & other){
				std::swap(m_target, other.m_target);
			}

			ComPtrType releaseOwnership() {
				ComPtrType toReturn = m_target;
				m_target = nullptr;
				return toReturn;
			}

			void reset(){
				swapTarget(nullptr);
			}

			CoTaskMemPtr<ComPtrType, ReleasePolicyClass>& operator=(ComPtrType toCopy) {
				swapTarget(toCopy);
				return *this;
			}

			CoTaskMemPtr<ComPtrType, ReleasePolicyClass>& operator=(CoTaskMemPtr<ComPtrType, ReleasePolicyClass>& toCopy) {
				swapTarget(toCopy.releaseOwnership());
				return *this;
			}

			template<class TargetClass>
			TargetClass & operator*() const{
				assert(m_target != nullptr);
				return *m_target;
			}

			ComPtrType operator->() const{
				assert(m_target != nullptr);
				return m_target;
			}

			bool operator==(const CoTaskMemPtr& other) const {
				return m_target == other.m_target;
			}

			template<class T>
			bool operator==(T pointer) const {
				return m_target == pointer;
			}

			bool operator!=(const CoTaskMemPtr& other) const {
				return m_target != other.m_target;
			}

			template<class T>
			bool operator!=(T pointer) const {
				return m_target != pointer;
			}

			operator ComPtrType() {
				return m_target;
			}

		protected:
			ComPtrType m_target;

			void releaseTarget(){
				if( m_target != nullptr ){
					ReleasePolicyClass::Release(m_target);
					m_target = nullptr;
				}
			}

			void swapTarget(const ComPtrType& newTarget){
				releaseTarget();
				m_target = newTarget;
			}
	};

}
}

