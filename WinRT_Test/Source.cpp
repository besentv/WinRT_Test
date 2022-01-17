// "C:\Program Files (x86)\Windows Kits\10\UnionMetadata\10.0.19041.0\Windows.winmd"

#include <stdio.h>
#include <comdef.h>
#include <winrt/base.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Capture.h>
#include <winrt/Windows.Media.SpeechRecognition.h>

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Data::Text;
using namespace winrt::Windows::System;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Media::Capture;
using namespace winrt::Windows::Media::SpeechRecognition;

bool recognizer_ready = false;
bool program_end = false;

void mediacapture_handler(IAsyncAction asyncInfo, AsyncStatus asyncStatus) {
	printf("%s status %#x, hres %#x\n", __FUNCDNAME__, asyncStatus, asyncInfo.ErrorCode());
}

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

int main() {
	winrt::init_apartment();

	MediaCaptureInitializationSettings settings;
	settings.StreamingCaptureMode(StreamingCaptureMode::Audio);
	settings.MediaCategory(MediaCategory::Speech);
	MediaCapture mc;
	IAsyncAction action = mc.InitializeAsync(settings);
	action.Completed(mediacapture_handler);

	//SpeechRecognizer sr(Windows::Globalization::Language::Language(hstring(L"en-us")));
	SpeechRecognizer sr;
	sr.Constraints().Append(SpeechRecognitionListConstraint(param::iterable<hstring>({ hstring(L"Who let the frogs out?"), hstring(L"What is going on heeeere?") }), hstring(L"test_text")));
	IAsyncOperation asy = sr.CompileConstraintsAsync();
	sr.StateChanged(speech_state_changed);
	asy.Completed(constraints_handler);
	
	while (!recognizer_ready);
	try {
		printf("Starting recognition.\n");
		IAsyncOperation asy2 = sr.RecognizeAsync();
		asy2.Completed(speech_handler);

	}
	catch (hresult_error ex)
	{
		//_com_error err(ex.code());
		printf("hresult %#x -> %ls\n", ex.code(), ex.message().c_str());
		



	}

	while (!program_end);
	getchar();


	return 0;
}