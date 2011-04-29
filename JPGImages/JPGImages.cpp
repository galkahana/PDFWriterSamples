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
#include "PDFImageXObject.h"
#include "DocumentContext.h"
#include "JPEGImageHandler.h"

// end pdfwriter library includes


DoubleAndDoublePair sGetImageDimensions(const JPEGImageInformation& inJPGImageInformation)
{
	DoubleAndDoublePair returnResult(1,1);

	do
	{
		// prefer JFIF determined resolution
		if(inJPGImageInformation.JFIFInformationExists)
		{
			double jfifXDensity = 	(0 == inJPGImageInformation.JFIFXDensity) ? 1:inJPGImageInformation.JFIFXDensity;
			double jfifYDensity = (0 == inJPGImageInformation.JFIFYDensity) ? 1:inJPGImageInformation.JFIFYDensity;

			switch(inJPGImageInformation.JFIFUnit)
			{
				case 1: // INCH
					returnResult.first = ((double)inJPGImageInformation.SamplesWidth / jfifXDensity) * 72.0;
					returnResult.second = ((double)inJPGImageInformation.SamplesHeight / jfifYDensity) * 72.0;
					break;
				case 2: // CM
					returnResult.first = ((double)inJPGImageInformation.SamplesWidth / jfifXDensity) * 72.0 / 2.54;
					returnResult.second = ((double)inJPGImageInformation.SamplesHeight / jfifYDensity) * 72.0 / 2.54;
					break;
				default: // 0 - aspect ratio
					returnResult.first = (double)inJPGImageInformation.SamplesWidth;
					returnResult.second = (double)inJPGImageInformation.SamplesHeight;
					break;
			}
			break;
		}

		// if no jfif try exif
		if(inJPGImageInformation.ExifInformationExists)
		{
			double exifXDensity = 	(0 == inJPGImageInformation.ExifXDensity) ? 1:inJPGImageInformation.ExifXDensity;
			double exifYDensity = (0 == inJPGImageInformation.ExifYDensity) ? 1:inJPGImageInformation.ExifYDensity;

			switch(inJPGImageInformation.ExifUnit)
			{
				case 1: // aspect ratio
					returnResult.first = (double)inJPGImageInformation.SamplesWidth;
					returnResult.second = (double)inJPGImageInformation.SamplesHeight;
					break;
				case 3: // CM
					returnResult.first = ((double)inJPGImageInformation.SamplesWidth / exifXDensity) * 72.0 / 2.54;
					returnResult.second = ((double)inJPGImageInformation.SamplesHeight / exifYDensity) * 72.0 / 2.54;
					break;
				default: // 2 - Inch
					returnResult.first = ((double)inJPGImageInformation.SamplesWidth / exifXDensity) * 72.0;
					returnResult.second = ((double)inJPGImageInformation.SamplesHeight / exifYDensity) * 72.0;
					break;
			}
			break;
		}

		// if no jfif, try photoshop
		if(inJPGImageInformation.PhotoshopInformationExists)
		{
			double photoshopXDensity = 	(0 == inJPGImageInformation.PhotoshopXDensity) ? 1:inJPGImageInformation.PhotoshopXDensity;
			double photoshopYDensity = (0 == inJPGImageInformation.PhotoshopYDensity) ? 1:inJPGImageInformation.PhotoshopYDensity;

			returnResult.first = ((double)inJPGImageInformation.SamplesWidth / photoshopXDensity) * 72.0;
			returnResult.second = ((double)inJPGImageInformation.SamplesHeight / photoshopYDensity) * 72.0;
			break;
		}

		// else aspect ratio
		returnResult.first = (double)inJPGImageInformation.SamplesWidth;
		returnResult.second = (double)inJPGImageInformation.SamplesHeight;
	}while(false);

	return returnResult;
}

static const wstring scBasePath =  L"..\\JPGImages\\Materials\\";

int wmain(int argc, wchar_t* argv[])
{
	PDFWriter pdfWriter;
	EStatusCode status;

	do
	{
		status = pdfWriter.StartPDF(scBasePath + L"JPGImages.pdf",ePDFVersion13);
		if(status != eSuccess)
			break;

		PDFPage* pdfPage = new PDFPage();
		pdfPage->SetMediaBox(PDFRectangle(0,0,595,842));

		// Place JPG, first as a form
		PDFFormXObject* image = pdfWriter.CreateFormXObjectFromJPGFile(scBasePath + L"SanAntonioPass.JPG");

		PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(pdfPage);

		pageContentContext->q();
		pageContentContext->cm(0.4,0,0,0.4,57.5,241);
		pageContentContext->Do(pdfPage->GetResourcesDictionary().AddFormXObjectMapping(image->GetObjectID()));
		pageContentContext->Q();

		delete image;		

		status = pdfWriter.EndPageContentContext(pageContentContext);
		if(status != eSuccess)
			break;

		status = pdfWriter.WritePageAndRelease(pdfPage);
		if(status != eSuccess)
			break;

		pdfPage = new PDFPage();
		pdfPage->SetMediaBox(PDFRectangle(0,0,595,842));

		// Now, as an image. determine dimensions based on library parsed information
		PDFImageXObject* imageA = pdfWriter.CreateImageXObjectFromJPGFile(scBasePath + L"SanAntonioPass.JPG");
		DoubleAndDoublePair dimensions = sGetImageDimensions(pdfWriter.GetDocumentContext().GetJPEGImageHandler().RetrieveImageInformation(scBasePath + L"SanAntonioPass.JPG").second);
		
		pageContentContext = pdfWriter.StartPageContentContext(pdfPage);

		pageContentContext->q();
		pageContentContext->cm(0.4 * dimensions.first,0,0,0.4 * dimensions.second,57.5,241);
		pageContentContext->Do(pdfPage->GetResourcesDictionary().AddImageXObjectMapping(imageA));
		pageContentContext->Q();


		delete imageA;

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
