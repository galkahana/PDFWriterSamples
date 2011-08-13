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

static const string scBasePath =  "..\\EmbeddingPDFs\\Materials\\";

using namespace PDFHummus;

EStatusCode EmbedAsPages()
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + "EmbedAsPages.PDF",ePDFVersion13);
		if(status != eSuccess)
			break;

		// appending all pages of XObjectContent.pdf
		status = pdfWriter.AppendPDFPagesFromPDF(scBasePath + "XObjectContent.PDF",PDFPageRange()).first;
		if(status != eSuccess)
			break;

		// appending pages 0,1,2,3,5,6,7,8,9,10 of BasicTIFFImagesTest.PDF
		PDFPageRange selectivePageRange;
		selectivePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		selectivePageRange.mSpecificRanges.push_back(ULongAndULong(0,3));
		selectivePageRange.mSpecificRanges.push_back(ULongAndULong(5,10));

		status = pdfWriter.AppendPDFPagesFromPDF(scBasePath + "BasicTIFFImagesTest.PDF",selectivePageRange).first;
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;

	}while(false);


	if(eSuccess == status)
		cout<<"Succeeded in creating EmbedAsPages.PDF file\n";
	else
		cout<<"Failed in creating EmbedAsPages.PDF file\n";

	return status;

}

EStatusCode UseAsXObjects()
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + "UseAsXObjects.PDF",ePDFVersion13);
		if(status != eSuccess)
			break;

		// creating XObjects for all pages of XObjectContent.pdf (2 pages)
		EStatusCodeAndObjectIDTypeList result = pdfWriter.CreateFormXObjectsFromPDF(scBasePath + "XObjectContent.PDF",PDFPageRange(),ePDFPageBoxMediaBox);
		if(result.first != eSuccess)
		{
			status = eFailure;
			break;
		}

		// determine page IDs
		ObjectIDTypeList::iterator it = result.second.begin();
		ObjectIDType firstPageID = *it;
		++it;
		ObjectIDType secondPageID = *it;

		// create a page using both xobjects
		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);

		// place the first page in the top left corner of the document
		contentContext->q();
		contentContext->cm(0.5,0,0,0.5,0,421);
		contentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(firstPageID));
		contentContext->Q();
		
		// draw a rectangle around the page bounds
		contentContext->G(0);
		contentContext->w(1);
		contentContext->re(0,421,297.5,421);
		contentContext->S();


		// place the second page in the bottom right corner of the document
		contentContext->q();
		contentContext->cm(0.5,0,0,0.5,297.5,0);
		contentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(secondPageID));
		contentContext->Q();

		// draw a rectangle around the page bounds
		contentContext->G(0);
		contentContext->w(1);
		contentContext->re(297.5,0,297.5,421);
		contentContext->S();


		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;


		// appending pages 0,1,2,3 of BasicTIFFImagesTest.PDF
		PDFPageRange selectivePageRange;
		selectivePageRange.mType = PDFPageRange::eRangeTypeSpecific;
		selectivePageRange.mSpecificRanges.push_back(ULongAndULong(0,3));

		result = pdfWriter.CreateFormXObjectsFromPDF(scBasePath + "BasicTIFFImagesTest.PDF",selectivePageRange,ePDFPageBoxMediaBox);
		if(result.first != eSuccess)
		{
			status = eFailure;
			break;
		}

		page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		contentContext = pdfWriter.StartPageContentContext(page);

		// placing them in all 4 corners of the 2nd page [going left right, top bottom]
		it = result.second.begin();
		contentContext->q();
		contentContext->cm(0.5,0,0,0.5,0,421);
		contentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(*it));
		contentContext->Q();
		
		++it;
		contentContext->q();
		contentContext->cm(0.5,0,0,0.5,297.5,421);
		contentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(*it));
		contentContext->Q();

		++it;
		contentContext->q();
		contentContext->cm(0.5,0,0,0.5,0,0);
		contentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(*it));
		contentContext->Q();

		++it;
		contentContext->q();
		contentContext->cm(0.5,0,0,0.5,297.5,0);
		contentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(*it));
		contentContext->Q();


		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;

	}while(false);


	if(eSuccess == status)
		cout<<"Succeeded in creating UseAsXObjects.PDF file\n";
	else
		cout<<"Failed in creating UseAsXObjects.PDF file\n";

	return status;
}

EStatusCode UseCopyingContext()
{
	PDFWriter pdfWriter;
	EStatusCode status;

	PDFDocumentCopyingContext* firstContext = NULL;
	PDFDocumentCopyingContext* secondContext = NULL;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + "UseCopyingContext.PDF",ePDFVersion13);
		if(status != eSuccess)
			break;

		firstContext = pdfWriter.CreatePDFCopyingContext(scBasePath + "XObjectContent.PDF");
		if(!firstContext)
		{
			status = eFailure;
			break;
		}
		 
		secondContext = pdfWriter.CreatePDFCopyingContext(scBasePath + "BasicTIFFImagesTest.PDF");
		if(!firstContext)
		{
			status = eFailure;
			break;
		}

		// appending pages, first from XObjectContent, then from BasicTIFFImageTest, and then again from XObjectContent
		status = firstContext->AppendPDFPageFromPDF(0).first;
		if(status != eSuccess)
			break;

		status = secondContext->AppendPDFPageFromPDF(0).first;
		if(status != eSuccess)
			break;

		status = firstContext->AppendPDFPageFromPDF(1).first;
		if(status != eSuccess)
			break;

		// placing pages as xobjects, from multiple PDFs
		EStatusCodeAndObjectIDType resultFirst = firstContext->CreateFormXObjectFromPDFPage(0,ePDFPageBoxMediaBox);
		if(resultFirst.first != eSuccess)
		{
			status = eFailure;
			break;
		}

		EStatusCodeAndObjectIDType resultSecond = secondContext->CreateFormXObjectFromPDFPage(0,ePDFPageBoxMediaBox);
		if(resultSecond.first != eSuccess)
		{
			status = eFailure;
			break;
		}

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		PageContentContext* contentContext = pdfWriter.StartPageContentContext(page);

		// placing the pages in a result page
		contentContext->q();
		contentContext->cm(0.5,0,0,0.5,0,421);
		contentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(resultFirst.second));
		contentContext->Q();
		
		contentContext->q();
		contentContext->cm(0.5,0,0,0.5,297.5,0);
		contentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(resultSecond.second));
		contentContext->Q();

		status = pdfWriter.EndPageContentContext(contentContext);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
			break;

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
			break;

	}while(false);

	delete firstContext;
	delete secondContext;
	return status;
}

int main(int argc, wchar_t* argv[])
{
	EStatusCode status;

	do
	{
		status = EmbedAsPages();
		if(status != eSuccess)
			break;

		status = UseAsXObjects();
		if(status != eSuccess)
			break;

		status = UseCopyingContext();
		if(status != eSuccess)
			break;

	}while(false);
	return 0;
}

