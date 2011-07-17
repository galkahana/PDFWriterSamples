// standard library includes
#include <iostream>
#include <string>
using namespace std;
// end standard library includes

// pdfwriter library includes
#include "PDFWriter.h"
#include "PDFEmbedParameterTypes.h"
#include "PageContentContext.h"
#include "PDFPage.h"
#include "PDFDocumentCopyingContext.h"

// end pdfwriter library includes

static const wstring scBasePath =  L"..\\MergingUniquePDFPages\\Materials\\";
static const wstring scSystemFontsPath = L"C:\\windows\\fonts\\";

EStatusCode SimplePageMerge()
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + L"SimplePageMerge.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PDFPageRange singePageRange;
		singePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		singePageRange.mSpecificRanges.push_back(ULongAndULong(0,0));

		status = pdfWriter.MergePDFPagesToPage(page,scBasePath + L"BasicTIFFImagesTest.PDF",singePageRange);
		if(status != eSuccess)
			break;

		PDFUsedFont* font = pdfWriter.GetFontForFile(scSystemFontsPath + L"arial.ttf");
		if(!font)
		{
			status = eFailure;
			break;
		}

		PageContentContext* pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->BT();
		pageContent->k(0,0,1,0);
		pageContent->Tf(font,30);
		pageContent->Tm(1,0,0,1,10,400);
		status = pageContent->Tj(L"Some written text");
		if(status != eSuccess)
			break;
		pageContent->ET();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;
	}while(false);

	
	return status;
}

EStatusCode UseCopyingContext()
{
	PDFWriter pdfWriter;
	EStatusCode status;

	// in this sample we'll create two pages, from 3 pages content of the merged page.
	// the first page will be used as a reusable object in both result pages. the second and third page will 
	// be reusables.

	do
	{
		status = pdfWriter.StartPDF(scBasePath + L"UseCopyingContext.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;

		PDFDocumentCopyingContext* copyingContext = pdfWriter.CreatePDFCopyingContext(scBasePath + L"BasicTIFFImagesTest.PDF");
		if(!copyingContext)
		{
			status = eFailure;
			break;
		}
		
		// create a reusable form xobject from the first page
		EStatusCodeAndObjectIDType result = copyingContext->CreateFormXObjectFromPDFPage(0,ePDFPageBoxMediaBox);
		if(result.first != eSuccess)
		{
			status = eFailure;
			break;
		}
		ObjectIDType reusableObjectID = result.second;

		// now let's begin constructing the pages

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,0);

		// merge unique page at lower left
		status = copyingContext->MergePDFPageToPage(page,1);
		if(status != eSuccess)
			break;

		pageContent->Q();

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,297.5,421);

		// place reusable object page on the upper right corner of the page
		pageContent->Do(page->GetResourcesDictionary().AddFormXObjectMapping(reusableObjectID));

		pageContent->Q();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;

		// now let's do the second page. similar, but with the second page as the unique content

		page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		pageContent = pdfWriter.StartPageContentContext(page);

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,0,0);

		// merge unique page at lower left
		status = copyingContext->MergePDFPageToPage(page,2);
		if(status != eSuccess)
			break;

		pageContent->Q();

		pageContent->q();
		pageContent->cm(0.5,0,0,0.5,297.5,421);

		// place reusable object page on the upper right corner of the page
		pageContent->Do(page->GetResourcesDictionary().AddFormXObjectMapping(reusableObjectID));

		pageContent->Q();

		status = pdfWriter.EndPageContentContext(pageContent);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;
	}while(false);

	return eSuccess;
}

int main(int argc, wchar_t* argv[])
{
	EStatusCode status;

	do
	{
		status = SimplePageMerge();
		if(status != eSuccess)
			break;

		status = UseCopyingContext();
		if(status != eSuccess)
			break;

	}while(false);
	return 0;
}
