// standard library includes
#include <iostream>
#include <string>
using namespace std;
// end standard library includes

// pdfwriter library includes
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "PDFFormXObject.h"
#include "XObjectContentContext.h"
// end pdfwriter library includes

static const string scBasePath =  "..\\DrawingShapes\\Materials\\";

using namespace PDFHummus;

int main(int argc, char* argv[])
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		// Initial Setup for file,page and page content
		status = pdfWriter.StartPDF(scBasePath + "DrawingShapes.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;

		PDFPage* pdfPage = new PDFPage();
		pdfPage->SetMediaBox(PDFRectangle(0,0,595,842));
		PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(pdfPage);

		// Start adding content to page

		// Draw a Line, stroke
		pageContentContext->q();
		pageContentContext->w(2);
		pageContentContext->K(0,0,1,0);

		pageContentContext->m(10,500);
		pageContentContext->l(30,700);
		pageContentContext->s();

		pageContentContext->Q();

		// Draw a Polygon, stroke
		pageContentContext->q();
		pageContentContext->w(2);
		pageContentContext->K(0,1,0,0);

		pageContentContext->m(40,500);
		pageContentContext->l(60,700);
		pageContentContext->l(160,700);
		pageContentContext->l(140,500);
		pageContentContext->s();

		pageContentContext->Q();

		// Draw a Rectangle, fill
		pageContentContext->q();
		pageContentContext->k(0,1,0,0);

		pageContentContext->re(200,400,100,300);
		pageContentContext->f();

		pageContentContext->Q();

		// Pausing content context, to allow form definition
		pdfWriter.PausePageContentContext(pageContentContext);

		// Create a form with a triangle pattern, RGB colorspace, fill
		PDFFormXObject* formXObject = pdfWriter.StartFormXObject(PDFRectangle(0,0,400,400));
		XObjectContentContext* xobjectContent = formXObject->GetContentContext();

		xobjectContent->rg(1,0,0);
		xobjectContent->m(0,0);
		xobjectContent->l(200,400);
		xobjectContent->l(400,0);
		xobjectContent->f();

		ObjectIDType formObjectID = formXObject->GetObjectID();
		status = pdfWriter.EndFormXObjectAndRelease(formXObject);
		if(status != eSuccess)
			break;

		// Place the form in multiple locations in the page
		string formNameInPage = pdfPage->GetResourcesDictionary().AddFormXObjectMapping(formObjectID);

		pageContentContext->q();
		pageContentContext->cm(0.5,0,0,0.5,120,100);
		pageContentContext->Do(formNameInPage);
		pageContentContext->Q();

		pageContentContext->q();
		pageContentContext->cm(0.2,0,0,0.2,350,100);
		pageContentContext->Do(formNameInPage);
		pageContentContext->Q();

		// End adding content to page

		// end page content, write page object and finalize PDF
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

