#include "MyForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
void Main(array<String^>^ args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	ROD_OMR_V1::MyForm myForm;        //NameOfProject::NameOfForm instanceOfForm;
	Application::Run(%myForm);
}