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
#include "TIFFUsageParameters.h"
// end pdfwriter library includes

using namespace PDFHummus;

static const string scBasePath =  "..\\PlayingWithTiffs\\Materials\\";

int main(int argc, char* argv[])
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + "PlayingWithTiffs.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;


		// Regaulr TIFF placement
		PDFPage* pdfPage = new PDFPage();
		pdfPage->SetMediaBox(PDFRectangle(0,0,595,842));
		PDFFormXObject* image = pdfWriter.CreateFormXObjectFromTIFFFile(scBasePath + "jim___ah.tif");
		if(!image)
		{
			status = eFailure;
			break;
		}
		PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(pdfPage);

		// Place the image in the bottom left of the page
		pageContentContext->q();
		pageContentContext->Do(pdfPage->GetResourcesDictionary().AddFormXObjectMapping(image->GetObjectID()));
		pageContentContext->Q();

		delete image;
		pdfWriter.EndPageContentContext(pageContentContext);
		pdfWriter.WritePageAndRelease(pdfPage);


		// Showing multiplepage tiff
		pdfPage = new PDFPage();
		pdfPage->SetMediaBox(PDFRectangle(0,0,595,842));
		
		TIFFUsageParameters tiffParams;
		tiffParams.PageIndex = 0;
		
		// Load first page
		image = pdfWriter.CreateFormXObjectFromTIFFFile(scBasePath + "multipage.tif",tiffParams);
		if(!image)
		{
			status = eFailure;
			break;
		}
		ObjectIDType imageAID = image->GetObjectID();
		delete image;

		// Load third page
		tiffParams.PageIndex = 2;
		image = pdfWriter.CreateFormXObjectFromTIFFFile(scBasePath + "multipage.tif",tiffParams);
		if(!image)
		{
			status = eFailure;
			break;
		}
		ObjectIDType imageBID = image->GetObjectID();
		delete image;

		pageContentContext = pdfWriter.StartPageContentContext(pdfPage);

		// place both image one next to each other
		pageContentContext->q();
		pageContentContext->cm(0.5,0,0,0.5,0,0);
		pageContentContext->Do(pdfPage->GetResourcesDictionary().AddFormXObjectMapping(imageAID));
		pageContentContext->Q();

		pageContentContext->q();
		pageContentContext->cm(0.5,0,0,0.5,312,0);
		pageContentContext->Do(pdfPage->GetResourcesDictionary().AddFormXObjectMapping(imageBID));
		pageContentContext->Q();

		pdfWriter.EndPageContentContext(pageContentContext);
		pdfWriter.WritePageAndRelease(pdfPage);


		// Showing Black and White stuff
		pdfPage = new PDFPage();
		pdfPage->SetMediaBox(PDFRectangle(0,0,595,842));

		// Load image as image mask
		TIFFUsageParameters tiffParamsA;
		tiffParamsA.BWTreatment.AsImageMask = 1;
		tiffParamsA.BWTreatment.OneColor = CMYKRGBColor(255,128,0);
		image = pdfWriter.CreateFormXObjectFromTIFFFile(scBasePath + "jim___ah.tif",tiffParamsA);
		if(!image)
		{
			status = eFailure;
			break;
		}
		string imageName = pdfPage->GetResourcesDictionary().AddFormXObjectMapping(image->GetObjectID());
		delete image;
		pageContentContext = pdfWriter.StartPageContentContext(pdfPage);

		// Place the image in the bottom left of the page
		pageContentContext->q();
		pageContentContext->Do(imageName);

		// Draw a rectangle
		pageContentContext->k(1,0,0,0);
		pageContentContext->re(159.36,0,159.36,195.12);
		pageContentContext->f();

		// Draw the mask on top of a rectangle, to show that it's transperent
		pageContentContext->cm(1,0,0,1,159.36,0);
		pageContentContext->Do(imageName);
		pageContentContext->Q();

		pdfWriter.EndPageContentContext(pageContentContext);
		pdfWriter.WritePageAndRelease(pdfPage);


		// Showing greyscale stuff
		pdfPage = new PDFPage();
		pdfPage->SetMediaBox(PDFRectangle(0,0,595,842));
		

		// grayscale purple
		TIFFUsageParameters tiffParamsB;
		tiffParamsB.GrayscaleTreatment.AsColorMap = true;
		tiffParamsB.GrayscaleTreatment.OneColor = CMYKRGBColor(255,255,0,0);
		tiffParamsB.GrayscaleTreatment.ZeroColor = CMYKRGBColor(0,0,0,0);
		image = pdfWriter.CreateFormXObjectFromTIFFFile(scBasePath + "jim___cg.tif",tiffParamsB);
		if(!image)
		{
			status = eFailure;
			break;
		}
		imageAID = image->GetObjectID();
		delete image;

		// grayscale cyan vs magenta
		tiffParamsB.GrayscaleTreatment.AsColorMap = true;
		tiffParamsB.GrayscaleTreatment.OneColor = CMYKRGBColor(255,0,0,0);
		tiffParamsB.GrayscaleTreatment.ZeroColor = CMYKRGBColor(0,255,0,0);
		image = pdfWriter.CreateFormXObjectFromTIFFFile(scBasePath + "jim___cg.tif",tiffParamsB);
		if(!image)
		{
			status = eFailure;
			break;
		}
		imageBID = image->GetObjectID();
		delete image;

		// plain grayscale
		image = pdfWriter.CreateFormXObjectFromTIFFFile(scBasePath + "jim___cg.tif");
		if(!image)
		{
			status = eFailure;
			break;
		}
		ObjectIDType imageCID = image->GetObjectID();
		delete image;

		pageContentContext = pdfWriter.StartPageContentContext(pdfPage);

		// placing the two images one next to each other
		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,0,646.88);
		pageContentContext->Do(pdfPage->GetResourcesDictionary().AddFormXObjectMapping(imageAID));
		pageContentContext->Q();

		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,159.36,646.88);
		pageContentContext->Do(pdfPage->GetResourcesDictionary().AddFormXObjectMapping(imageBID));
		pageContentContext->Q();

		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,318.72,646.88);
		pageContentContext->Do(pdfPage->GetResourcesDictionary().AddFormXObjectMapping(imageCID));
		pageContentContext->Q();


		pdfWriter.EndPageContentContext(pageContentContext);
		pdfWriter.WritePageAndRelease(pdfPage);

		status = pdfWriter.EndPDF();
	}while(false);


	if(eSuccess == status)
		cout<<"Succeeded in creating PDF file\n";
	else
		cout<<"Failed in creating PDF file\n";

	return 0;
}
