#pragma once

#include "ref_counted.h"

namespace AsyncCall
{
	class ICallback : public IRefCounted
	{
	public:
		virtual void Run() = 0;

	protected:
		~ICallback() {}
	};

	typedef boost::intrusive_ptr<ICallback> CallbackPtr;

	namespace Detail
	{
		template<class AsyncController>
		class CallbackImpl : public RefCountedImpl<ICallback, true>
		{
		public:
			CallbackImpl(
				const boost::weak_ptr<AsyncController>& pController,
				const boost::function<void (AsyncController*)>& func) :
				m_pController(pController), m_func(func)
			{
			}

			virtual void Run()
			{
				if (boost::shared_ptr<AsyncController> pController = m_pController.lock())
					m_func(pController.get());
			}

		private:
			boost::weak_ptr<AsyncController> m_pController;
			boost::function<void (AsyncController*)> m_func;
		};

		class MainThreadCallbackImpl : public main_thread_callback
		{
		public:
			MainThreadCallbackImpl(const CallbackPtr& callback) : m_callback(callback) {}

			virtual void callback_run()
			{
				m_callback->Run();
			}

		private:
			CallbackPtr m_callback;
		};

	} // namespace Detail

	template<class AsyncController>
	CallbackPtr MakeCallback(
		const boost::weak_ptr<AsyncController>& pController,
		const boost::function<void (AsyncController*)>& func)
	{
		return CallbackPtr(new Detail::CallbackImpl<AsyncController>(pController, func));
	}

	inline void AsyncRunInMainThread(const CallbackPtr& callback)
	{
		typedef Detail::MainThreadCallbackImpl CallbackImpl;

 		service_ptr_t<CallbackImpl> myc = new service_impl_t<CallbackImpl>(callback);
		static_api_ptr_t<main_thread_callback_manager>()->add_callback(myc);
	}

} // namespace AsyncCall