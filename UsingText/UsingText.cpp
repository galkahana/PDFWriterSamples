// standard library includes
#include <iostream>
#include <string>
using namespace std;
// end standard library includes

// pdfwriter library includes
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"

// end pdfwriter library includes

static const wstring scBasePath =  L"..\\UsingText\\Materials\\";
static const wstring scSystemFontsPath = L"C:\\windows\\fonts\\";

int main(int argc, wchar_t* argv[])
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + L"UsingText.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;

		// Create a new page
		PDFPage* pdfPage = new PDFPage();
		pdfPage->SetMediaBox(PDFRectangle(0,0,595,842));

		// Create a content context for the page
		PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(pdfPage);

		// basic text placement, with positioning (tm and td). and setting the font
		PDFUsedFont* arialTTF = pdfWriter.GetFontForFile(scSystemFontsPath + L"arial.ttf");
		if(!arialTTF)
		{
			status = eFailure;
			break;
		}

		pageContentContext->BT();
		pageContentContext->k(0,0,0,1);
		pageContentContext->Tf(arialTTF,20);
		pageContentContext->Tm(1,0,0,1,90,610);
		pageContentContext->Tj(L"San Antonio Pass, Cordillera Huayhuash, Peru");
		pageContentContext->ET();				

		// text with width control

		// multline text with leading control

		// some text rendering sample

		// diect glyph placement

		// End content context, and write the page
		status = pdfWriter.EndPageContentContext(pageContentContext);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(pdfPage);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
	}while(false);

	if(eSuccess == status)
		cout<<"Succeeded in creating PDF file\n";
	else
		cout<<"Failed in creating PDF file\n";

	return 0;
}