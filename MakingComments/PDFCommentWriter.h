#pragma once

#include "PDFComment.h"
#include "ObjectsBasicTypes.h"
#include "DocumentContextExtenderAdapter.h"
#include <map>
#include <utility>

using namespace std;

class PDFWriter;

typedef map<PDFComment*,ObjectIDType> PDFCommentToObjectIDTypeMap;
typedef pair<EStatusCode,ObjectIDType> EStatusCodeAndObjectIDType;

class PDFCommentWriter : public DocumentContextExtenderAdapter
{
public:
	PDFCommentWriter(PDFWriter* inPDFWriter);
	~PDFCommentWriter(void);


	// Write a comment (now) and attach it to the page that will be written next.
	// Technically speaking, the comment will be written at this point (so don't have an open content stream, or something),
	// as well as any comments that this one is connected to through "In Reply To" relationship.
	// Important: The comment will become owned by the PDFCommentWriter, so no need to delete it.
	// it will be deleted when the page is written.
	EStatusCode AttachCommentToNextPage(PDFComment* inComment);


	// IDocumentContextExtenderAdapter implementation

	// implementing page write to write references from the page dictionary to the comments page
	// It will also delete the comments at this point.
	virtual EStatusCode OnPageWrite(
						PDFPage* inPage,
						DictionaryContext* inPageDictionaryContext,
						ObjectsContext* inPDFWriterObjectContext,
						DocumentContext* inPDFWriterDocumentContext);

private:
	PDFWriter* mPDFWriter;
	PDFCommentToObjectIDTypeMap mCommentsForNextPage;

	void ListenOnPageWrites();
	EStatusCodeAndObjectIDType WriteCommentsTree(PDFComment* inComment);
	void CleanupComments();
	
};
