// standard library includes
#include <iostream>
#include <string>
using namespace std;
// end standard library includes

// pdfwriter library includes
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "PDFFormXObject.h"


// end pdfwriter library includes

static const string scBasePath =  "..\\Links\\Materials\\";
static const string scSystemFontsPath = "C:\\windows\\fonts\\";

using namespace PDFHummus;

int main(int argc, wchar_t* argv[])
{
	EStatusCode status;
	PDFWriter pdfWriter;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + "Links.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFFormXObject* soundCloudLogo = pdfWriter.CreateFormXObjectFromJPGFile(scBasePath + "soundcloud_logo.jpg");

		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);
		if(!contentContext)
			break;

		PDFUsedFont* font = pdfWriter.GetFontForFile(scSystemFontsPath + "arial.ttf");
		if(!font)
			break;

		// Draw some text
		contentContext->BT();
		contentContext->k(0,0,0,1);
		contentContext->Tf(font,1);
		contentContext->Tm(11,0,0,11,90.024,709.54);
		contentContext->Tj("http://pdfhummus.com");
		contentContext->ET();

		// Draw soundcloud loog
		contentContext->q();
		contentContext->cm(0.5,0,0,0.5,90.024,200);
		contentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(soundCloudLogo->GetObjectID()));
		contentContext->Q();
		
		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != eSuccess)
			break;

		delete soundCloudLogo;

		// now let's attach some links.

		// first, the link for the test:
		pdfWriter.AttachURLLinktoCurrentPage("http://www.pdfhummus.com",PDFRectangle(87.75,694.56,198.76,720));

		// second, link for the logo.
		pdfWriter.AttachURLLinktoCurrentPage("http://www.soundcloud.com",PDFRectangle(90.024,200,367.524,375));

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
			break;
	}while(false);
	return 0;
}

