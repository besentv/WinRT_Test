// "C:\Program Files (x86)\Windows Kits\10\UnionMetadata\10.0.19041.0\Windows.winmd"

#include <stdio.h>
#include <comdef.h>
#include <winrt/base.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.SpeechRecognition.h>

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Data::Text;
using namespace winrt::Windows::System;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Media::SpeechRecognition;


bool recognizer_ready = false;
bool program_end = false;

//void mediacapture_handler(IAsyncAction asyncInfo, AsyncStatus asyncStatus) {
//	printf("%s status %#x, hres %#x\n", __FUNCDNAME__, asyncStatus, asyncInfo.ErrorCode());
//	//recognizer_ready = true;
//}

void constraints_handler(IAsyncOperation<SpeechRecognitionCompilationResult> asyncInfo, AsyncStatus asyncStatus) {
	printf("%s status %#x, hres %#x\n", __FUNCDNAME__, (int)asyncInfo.get().Status(), asyncInfo.ErrorCode());
	recognizer_ready =  true;
}

void speech_handler(IAsyncOperation<SpeechRecognitionResult> asyncInfo, AsyncStatus asyncStatus) {
	printf("%s speech done: status %#x, hres %#x\n", __FUNCDNAME__, asyncStatus, asyncInfo.ErrorCode());

	printf("RECOGNIZED TEXT: %ls\n", asyncInfo.GetResults().Text().c_str());

	program_end = true;
}

void speech_state_changed(SpeechRecognizer sender, SpeechRecognizerStateChangedEventArgs args) {
	printf("%s state %#x\n", __FUNCDNAME__, (int)args.State());
}

void result_generated(SpeechContinuousRecognitionSession sender, SpeechContinuousRecognitionResultGeneratedEventArgs args) {
	printf("%s state %#x, text %ls\n", __FUNCDNAME__, (int)args.Result().Status(), args.Result().Text().c_str());
}

void completed(SpeechContinuousRecognitionSession sender, SpeechContinuousRecognitionCompletedEventArgs args) {
	printf("%s state %#x\n", __FUNCDNAME__, (int)args.Status());
}

void recognition_handler(IAsyncAction asyncInfo, AsyncStatus asyncStatus) {
	printf("ENDED! status %#x, hres %#x\n", asyncStatus, asyncInfo.ErrorCode());
	//recognizer_ready = true;
}

int main() {
	winrt::init_apartment();

	SpeechRecognitionListConstraint srlc = SpeechRecognitionListConstraint(param::iterable<hstring>({ hstring(L"Who let the frogs out?"), hstring(L"What is going on heeeere?"), hstring(L"A number 9 large.") }), hstring(L"test_text"));
	//SpeechRecognizer sr(Windows::Globalization::Language::Language(hstring(L"en-us")));
	SpeechRecognizer sr;
	//sr.StateChanged(speech_state_changed);
	SpeechContinuousRecognitionSession crs = sr.ContinuousRecognitionSession();
	//crs.Completed(completed);
	crs.ResultGenerated(result_generated);
	sr.Constraints().Append(srlc);

	SpeechRecognitionListConstraint srlc2 = SpeechRecognitionListConstraint(param::iterable<hstring>({ hstring(L"Give us a sign."), hstring(L"Hello"), hstring(L"Where are you?") }), hstring(L"test_text2"));

	//IAsyncOperation asy = sr.CompileConstraintsAsync();
	//asy.Completed(constraints_handler);

	IAsyncOperation asy = sr.CompileConstraintsAsync();
	asy.Completed(constraints_handler);
	while (!recognizer_ready);
	
	try {
		printf("Starting recognition.\n");
		//IAsyncOperation asy2 = sr.RecognizeAsync();
		//asy2.Completed(speech_handler);

		printf("%s state %#x\n", __FUNCDNAME__, (int)asy.Status());
		IAsyncAction asy2 = crs.StartAsync();
		asy2.Completed(recognition_handler);

	}
	catch (hresult_error ex)
	{
		printf("hresult %#x -> %ls\n", ex.code(), ex.message().c_str());

	}

	
	while (!program_end);
	getchar();


	return 0;
}