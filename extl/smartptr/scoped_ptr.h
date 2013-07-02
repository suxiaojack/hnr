/* ///////////////////////////////////////////////////////////////////////
 * File:		scoped_ptr.h		
 *
 * Created:		08.02.25				
 * Updated:		08.05.06
 *
 * Brief: scoped_ptr class - scoped pointer
 * 
 * [<Home>]
 * Copyright (c) 2008-2020, Waruqi All rights reserved.
 * //////////////////////////////////////////////////////////////////// */
#ifndef EXTL_SMART_PTR_SCOPED_PTR_H
#define EXTL_SMART_PTR_SCOPED_PTR_H

/*!\file scoped_ptr.h
 * \brief scoped_ptr class - scoped pointer
 *
 * - safe implicit conversion to the bool type
 * - custom deleter
 *   - default deleter: new_allocator<T>::object_deleter_type
 *   - custom deleter function object:
 *     struct deleter
 *     {
 *         void operator()(int *p)
 *         {
 *           delete p;
 *         }
 *     };
 *   - usage: 
 *     - scoped_ptr<int, deleter > p(new int); - using default deleter 
 *     - scoped_ptr<int, deleter > p(new int, deleter());
 *     - scoped_ptr<int, deleter > p(new int, deleter(...));
 *
 */
#ifndef __cplusplus
#	error scoped_ptr.h need be supported by c++.
#endif

/* ///////////////////////////////////////////////////////////////////////
 * Includes
 */
#include "prefix.h"
#include "../utility/operator_bool.h"
#include "../memory/allocator_selector.h"
#include "../counter/shared_counter.h"
#include "../algorithm/algorithm.h" /* for std_swap */

/* ///////////////////////////////////////////////////////////////////////
 * Warnings
 */
#if defined(EXTL_COMPILER_IS_MSVC) && (_MSC_VER <= 1200)
#	pragma warning(push)
#	pragma warning(disable:4284) /* odd return type for operator-> */
#endif

/* ///////////////////////////////////////////////////////////////////////
 * ::extl namespace
 */
EXTL_BEGIN_NAMESPACE

/*!\brief scoped_ptr class
 * 
 * \param T The value type 
 * \param D The deleter type
 *
 * \ingroup extl_group_smart_ptr
 */
template<	typename_param_k T
	#ifdef EXTL_TEMPLATE_CLASS_DEFAULT_ARGUMENT_SUPPORT	
		,	typename_param_k D = typename_type_def_k new_allocator<T>::object_deleter_type	
	#else
		,	typename_param_k D												
	#endif
		>
class scoped_ptr 
	: protected D			
{
	/// \name Types
	/// @{
	public:
		typedef scoped_ptr<T, D>				class_type;			
		typedef T								value_type;			
		typedef value_type*						pointer;			
		typedef value_type const*				const_pointer;		
		typedef value_type&						reference;			
		typedef value_type const&				const_reference;	
		typedef D								deleter_type;
	/// @}

	private:
		/// The pointer
		pointer									m_ptr;		

	private:
		/* Prohibit copy constructor and assignment 
		 * Note: 
		 * Maybe EBO is not supported if uses extl::uncopyable,
		 * because some compilers do not support EBO_FORM_6 - EXTL_EBO_FORM_6_SUPPORT
		 */
		scoped_ptr(class_type const&);
		class_type& operator=(class_type const&);

		/* Member template friend */
	#ifdef EXTL_MEMBER_TEMPLATE_FRIEND_SUPPORT
		template < typename_param_k T1, typename_param_k D1 >
		friend class scoped_ptr;
	#endif

	/// \name Constructors
	/// @{
	public:
		/* Constructor and destructor */
		scoped_ptr()
			: m_ptr(NULL), deleter_type()
		{
		}
		~scoped_ptr() EXTL_THROW_0()
		{
			EXTL_ASSERT(is_valid());
			/* Destroys the resource */
			deleter()(m_ptr);
		}

		explicit_k scoped_ptr(pointer p)
			: m_ptr(p), deleter_type()
		{
			EXTL_ASSERT(is_valid());
		}

		scoped_ptr(pointer p, deleter_type const& del)
			: m_ptr(p), deleter_type(del)
		{
			EXTL_ASSERT(is_valid());
		}

	#ifdef EXTL_MEMBER_TEMPLATE_CTOR_OVERLOAD_DISCRIMINATED_SUPPORT
		template < typename_param_k T1 >
		explicit_k scoped_ptr(T1* p)
			: m_ptr(p), deleter_type()
		{
			EXTL_ASSERT(is_valid());
		}
	#endif
	/// @}

	/// \name Accessors
	/// @{
	public:
		/* Dereference */
		const_reference operator*() const
		{
			EXTL_ASSERT(NULL != m_ptr);
			EXTL_ASSERT(is_valid());
			return *m_ptr;
		}

		reference operator*()
		{
			EXTL_ASSERT(NULL != m_ptr);
			EXTL_ASSERT(is_valid());
			return *m_ptr;
		}
		const_pointer operator->() const
		{
			EXTL_ASSERT(NULL != m_ptr);
			EXTL_ASSERT(is_valid());
			return m_ptr;
		}

		pointer operator->()
		{
			EXTL_ASSERT(NULL != m_ptr);
			EXTL_ASSERT(is_valid());
			return m_ptr;
		}

		const_pointer get_ptr() const
		{
			EXTL_ASSERT(is_valid());
			return m_ptr;
		}

		pointer get_ptr()
		{
			EXTL_ASSERT(is_valid());
			return m_ptr;
		}
	/// @}
	/// \name Mutators
	/// @{
	public:
		/* Swap */
		void swap(class_type& other)
		{
			EXTL_ASSERT(is_valid());
			std_swap(m_ptr, other.m_ptr);
			EXTL_ASSERT(is_valid());
		}
	#ifdef EXTL_MEMBER_TEMPLATE_FUNC_OVERLOAD_DISCRIMINATED_SUPPORT
		template < typename_param_k T1, typename_param_k D1 >
		void swap(scoped_ptr<T1, D1>& other)
		{
			EXTL_ASSERT(is_valid());
			std_swap(m_ptr, other.m_ptr);
			EXTL_ASSERT(is_valid());
		}
	#endif
		/* Reset the pointer */
		void reset()
		{
			class_type().swap(*this);
		}	
		void reset(pointer p)
		{
			EXTL_ASSERT(p != m_ptr); 
			class_type(p).swap(*this);
		}
	#ifdef EXTL_MEMBER_TEMPLATE_FUNC_OVERLOAD_DISCRIMINATED_SUPPORT
		template < typename_param_k T1 >
		void reset(T1* p)
		{
			EXTL_ASSERT(p != m_ptr); 
			class_type(p).swap(*this);
		}
	#endif
	/// @}

	public:
		/// safe implicit conversion to the bool type 
		EXTL_OPERATOR_BOOL_DEFINE_TYPE_T(class_type, safe_bool_type);
		operator safe_bool_type() const
		{
			return EXTL_OPERATOR_BOOL_RETURN_RESULT(m_ptr != NULL);
		}
		/// operator!() overload
		e_bool_t operator !() const							{ return (m_ptr == NULL);	}

		/// less than 
		e_bool_t lt(class_type const & rhs) const			{ return m_ptr < rhs.m_ptr;	}
	#ifdef EXTL_MEMBER_TEMPLATE_FUNC_OVERLOAD_DISCRIMINATED_SUPPORT
		/// less than 
		template < typename_param_k T1, typename_param_k D1 >
		e_bool_t lt(scoped_ptr<T1, D1> const & rhs) const	{ return m_ptr < rhs.m_ptr;	}
	#endif

	private:
		e_bool_t is_valid() const				{ return e_true_v;			}
		deleter_type& deleter()				{ return *this;				}
		deleter_type const& deleter() const { return *this;				}
};

/* ///////////////////////////////////////////////////////////////////////
 * void specialization
 */
#ifdef EXTL_TEMPLATE_PARTIAL_SPEC_SUPPORT
template< typename_param_k D >
class scoped_ptr<void, D > 
	: protected D
#else
EXTL_TEMPLATE_SPECIALISATION
class scoped_ptr<void, new_allocator<void>::object_deleter_type >
	: protected new_allocator<void>::object_deleter_type
#endif
{
	public:
		typedef void										value_type;				
		typedef value_type*									pointer;				
		typedef value_type const*							const_pointer;			
		typedef value_type									reference;				
		typedef value_type									const_reference;		
	#ifdef EXTL_TEMPLATE_PARTIAL_SPEC_SUPPORT
		typedef D											deleter_type;			
	#else
		typedef new_allocator<void>::object_deleter_type	deleter_type;		
	#endif
		typedef scoped_ptr<void, deleter_type>				class_type;			

	private:
		pointer												m_ptr;	
	
	private:
		/* Prohibit copy constructor and assignment 
		 * Note: 
		 * Maybe EBO is not supported if uses extl::uncopyable,
		 * because some compilers do not support EBO_FORM_6 - EXTL_EBO_FORM_6_SUPPORT
		 */
		scoped_ptr(class_type const&);
		class_type& operator=(class_type const&);

		/* Member template friend */
	#ifdef EXTL_MEMBER_TEMPLATE_FRIEND_SUPPORT
		template < typename_param_k T1, typename_param_k D1 >
		friend class scoped_ptr;
	#endif
	
	public:
		scoped_ptr()
			: m_ptr(NULL), deleter_type()
		{
		}

		explicit_k scoped_ptr(pointer p)
			: m_ptr(p), deleter_type()
		{
			EXTL_ASSERT(is_valid());
		}

		scoped_ptr(pointer p, deleter_type const& del)
			: m_ptr(p), deleter_type(del)
		{
			EXTL_ASSERT(is_valid());
		}
		~scoped_ptr() EXTL_THROW_0()
		{
			EXTL_ASSERT(is_valid());
			deleter()(m_ptr);
		}
	#ifdef EXTL_MEMBER_TEMPLATE_CTOR_OVERLOAD_DISCRIMINATED_SUPPORT
		template < typename_param_k T1 >
		explicit_k scoped_ptr(T1* p)
			: m_ptr(p), deleter_type()
		{
			EXTL_ASSERT(is_valid());
		}
	#endif

	public:
		const_pointer get_ptr() const
		{
			EXTL_ASSERT(is_valid());
			return m_ptr;
		}

		pointer get_ptr()
		{
			EXTL_ASSERT(is_valid());
			return m_ptr;
		}
		/* swap */
		void swap(class_type& other)
		{
			EXTL_ASSERT(is_valid());
			std_swap(m_ptr, other.m_ptr);
			EXTL_ASSERT(is_valid());
		}
	#ifdef EXTL_MEMBER_TEMPLATE_FUNC_OVERLOAD_DISCRIMINATED_SUPPORT
		template < typename_param_k T1, typename_param_k D1 >
		void swap(scoped_ptr<T1, D1>& other)
		{
			EXTL_ASSERT(is_valid());
			std_swap(m_ptr, other.m_ptr);
			EXTL_ASSERT(is_valid());
		}
	#endif

		/* Reset the pointer */
		void reset()
		{
			class_type().swap(*this);
		}	
		void reset(pointer p)
		{
			EXTL_ASSERT(p != m_ptr); 
			class_type(p).swap(*this);
		}
	#ifdef EXTL_MEMBER_TEMPLATE_FUNC_OVERLOAD_DISCRIMINATED_SUPPORT
		template < typename_param_k T1 >
		void reset(T1* p)
		{
			EXTL_ASSERT(p != m_ptr); 
			class_type(p).swap(*this);
		}
	#endif

	public:
		/* safe implicit conversion to the bool type */
	#ifdef EXTL_TEMPLATE_PARTIAL_SPEC_SUPPORT
		EXTL_OPERATOR_BOOL_DEFINE_TYPE_T(class_type, safe_bool_type);
	#else
		EXTL_OPERATOR_BOOL_DEFINE_TYPE(class_type, safe_bool_type);
	#endif
		operator safe_bool_type() const
		{
			return EXTL_OPERATOR_BOOL_RETURN_RESULT(m_ptr != NULL);
		}

		e_bool_t operator !() const							{ return (m_ptr == NULL);	}

		/* less than */
		e_bool_t lt(class_type const & rhs) const			{ return m_ptr < rhs.m_ptr;	}
	#ifdef EXTL_MEMBER_TEMPLATE_FUNC_OVERLOAD_DISCRIMINATED_SUPPORT
		template < typename_param_k T1, typename_param_k D1 >
		e_bool_t lt(scoped_ptr<T1, D1> const & rhs) const	{ return m_ptr < rhs.m_ptr;	}
	#endif

	private:
		e_bool_t is_valid() const				{ return e_true_v;			}
		deleter_type& deleter()				{ return *this;				}
		deleter_type const& deleter() const { return *this;				}
};


/* ///////////////////////////////////////////////////////////////////////
 * Operators overload
 */
template< typename_param_k T, typename_param_k D >
inline e_bool_t operator==(scoped_ptr<T, D> const& lhs, scoped_ptr<T, D> const& rhs)
{
    return lhs.get_ptr() == rhs.get_ptr();
}

template< typename_param_k T, typename_param_k D >
inline e_bool_t operator!=(scoped_ptr<T, D> const& lhs, scoped_ptr<T, D> const& rhs)
{
    return lhs.get_ptr() != rhs.get_ptr();
}

/* VC6.0: p != NULL */
#if defined(EXTL_COMPILER_IS_MSVC) && (_MSC_VER == 1200)
template< typename_param_k T, typename_param_k D >
inline e_bool_t operator==(scoped_ptr<T, D> const& lhs, void* rhs)
{
    return lhs.get_ptr() == static_cast<T*>(rhs);
}

template< typename_param_k T, typename_param_k D >
inline e_bool_t operator!=(scoped_ptr<T, D> const& lhs, void* rhs)
{
    return lhs.get_ptr() != static_cast<T*>(rhs);
}
#endif

template< typename_param_k T, typename_param_k D >
inline e_bool_t operator<(scoped_ptr<T, D> const& lhs, scoped_ptr<T, D> const& rhs)
{
    return lhs.lt(rhs);
}

/* swap */
template< typename_param_k T, typename_param_k D >
inline void swap(scoped_ptr<T, D>& lhs, scoped_ptr<T, D>& rhs)
{
    lhs.swap(rhs);
}

/* get pointer shim */
template< typename_param_k T, typename_param_k D >
inline T const* get_ptr(scoped_ptr<T, D> const& p)
{
    return p.get_ptr();
}


/* ///////////////////////////////////////////////////////////////////////
 * Unit-testing 
 */
#ifdef EXTL_SCOPED_PTR_TEST_ENABLE
#	include "unit_test/scoped_ptr_test.h"
#endif

/* ///////////////////////////////////////////////////////////////////////
 * ::extl namespace
 */
EXTL_END_NAMESPACE

/* ///////////////////////////////////////////////////////////////////////
 * Warnings
 */
#if defined(EXTL_COMPILER_IS_MSVC) && (_MSC_VER <= 1200)
#	pragma warning(pop)
#endif  

/* ///////////////////////////////////////////////////////////////////////
 * std::swap 
 */
#if !defined(EXTL_NO_STL) && \
		!defined(EXTL_NO_NAMESPACE)
/* ::std namespace */
EXTL_STD_BEGIN_NAMESPACE

template< typename_param_k T, typename_param_k D >
inline void swap(EXTL_NS(scoped_ptr)<T, D>& lhs, EXTL_NS(scoped_ptr)<T, D>& rhs)
{
    lhs.swap(rhs);
}
/* ::std namespace */
EXTL_STD_END_NAMESPACE
#endif

/* //////////////////////////////////////////////////////////////////// */
#endif /* EXTL_SMART_PTR_SCOPED_PTR_H */
/* //////////////////////////////////////////////////////////////////// */

