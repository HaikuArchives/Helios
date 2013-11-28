#include "CDText.h"


int main() {

	CDTextFile *cdtext=new CDTextFile("/boot/home/cdtext.test", B_READ_WRITE  |  B_CREATE_FILE);

	cdtext->SetTracks(8);
	cdtext->SetAlbum("STEREOPLAY SOUNDCHECK");
	cdtext->SetTrack(1, "DOUG MAC LEOD:WHOSE TRUTH,WHOSE LIES?");
	cdtext->SetTrack(2, "JOE BEARD:THE BITTER SEED");
	cdtext->SetTrack(3, "TERRY EVANS:THE RIVER");
	cdtext->SetTrack(4, "THE BRUCE KATZ BAND:BEEF JERKY");
	cdtext->SetTrack(5, "LARRY GARNER:THE MUDDY RIVER");
	cdtext->SetTrack(6, "DAVID JOHANSEN:WELL,I'VE BEEN TO MEMPHIS");
	cdtext->SetTrack(7, "ERIC BIBB:WORLD WAR BLUES");
	cdtext->SetTrack(8, "SCOTT HOLT:DARK OF THE NIGHT");
	cdtext->FlushAll();
	delete cdtext;	
	return 0;
}