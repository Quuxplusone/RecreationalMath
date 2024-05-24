# Draw an SVG image of the circles with the given radii `rs`.

import math

# Seven circles
#rs = [74093013264220963696985951723621824993401496106712599843745632757868878955201103, 72333717592370716909678992308325894081276333001351846678740385929934472106164969, 60204683051421405769051945835793047761638493927210983906532124747674896182870528, 59053642225625298408200564298992174001806360919401037297200799988263687788792297, 56569831097890384789474384712378796781934697055290321511757589546475143370566600, 52414344984987851746538138642673843291497921924501122275485197586188094863142022, 50484123201703498052246834768677968358769236695379009854095268259325694165739400, 49628896094888303582449176775589025562125343803785565233441227696481017030428872]
#rs = [589187107184205258013835221942180567316210205430075419905603933704392554696565, 566256364344062745544171765215757438759658789938239650154372886292220254408439, 485725480899976231159077269663456831410556134756277713007197696833246432788736, 462947428811742131132160903916393443838870308330873550865327662050186432994935, 456219867539510916410781263039595623559554033685299227971892168650636765908500, 440997367112358008439325977563224490016956596209868221194177773351882638198548, 424958411337650594456095501170114536575789653702775096883983427501254434015060, 354663383805403751120568013393682153665279654013403096994778184967813682716500]
#rs = [5842237069239620197942294402732301440312823276896154, 5470527250244335919386387876021500592589609860168064, 4935879765308918878126300275056756362337709761280707, 4655007533492060194387838247877541755165326973143075, 4461826145970995530666901866206622655744544961880771, 4131714032353139387764683399994487013206654242553184, 3981775022042443136709511443344433286153265532257671, 3912167037159821565970509521571839879167302848956800]
#rs = [1724796815282763300398962628902721020766279804523118, 1539667493598837942179553316266965855428595965837384, 1526678413220936741448746715134237756651384357426848, 1372928063786178355249408412201052830199546888842482, 1318545209337553165761686422682811123501448401674400, 1218650422460589938697177121516274008690040390502507, 1176628474614455765623027900044803436133015105309600, 1153918924245194976874841450508455194540751092601032]
#rs = [86309248126780954337804371685081554995705511, 79706577901178792510839262644233499768485114, 77389801055800459164013871533546220839007000, 76057369708323280744042545954407603716971864, 70721835463916546381975088249114202006525569, 57130662460589425172804128356827474523264384, 56572976785006820003398583867200328433858369, 51479861698842760543141738248868680621647000]
#rs = [19433533385617744852851583428813999957196, 18763603212116791660617158019092105672059, 15952488504863868163798074109141199077120, 15930663042601063008633427836449960188980, 15095258908090731219533050046881904578304, 14886516497029821945881074463075285621184, 13703720524618851436189847181849116962399, 11178109337500526916337377581711749017024]
#rs = [8488578531134275835759037494021723175, 8139191477001249351777420142929304350, 7013205259796752989035561264887005600, 6964989074185003788542297501554116200, 6539445604789493124513168219911999950, 5953160585766439013517856620876372264, 5833490848661779309553968899206509950, 5479451851081666902151355001045452475]
#rs = [1023293178368872289845522242814129926, 924285577421970954531038238457557536, 895243165461980102686091543967656488, 871539979371831414861889312801290024, 845511889492496571686050189901345106, 754546713369037141584080893744992168, 635233640157227870178505819858503456, 621790300933539364312535091081556899]
#rs = [496750400852379068750254754918373037, 496631943998389656931573371983984000, 488705859947887100025554273057346753, 484146097719573957429164211634148960, 404256475391838659100182536414473378, 330885316426568642238526737779898648, 327014502843611468389677631834732928, 225396486471542354681528048649651963]
#rs = [19849928693561210643960383705454462, 19762344279715183720410347994440538, 18199600765454631678943172642716800, 17113717966522320797355163059980613, 16206226732750885227565832833854069, 14805196711479374505560692098486669, 14366736306812578780218882552019488, 9025120472078837841529592293969024]
#rs = [3351747005653424648383741138532, 3205723049598255148555654977125, 2775693993058987197622141299968, 2750071286250985696293256343360, 2576265167388890063586881775735, 2355770023351368908545664347968, 2298393837394654407377433698465, 2168159590729803674819757284343]
rs = [2941564115506288009572255050208, 2641663701806844064724293810176, 2588185711710249680183593951791, 2354857694119310401212561211842, 2236188368634576996067318957950, 2089644243395382075169644129792, 1996023217448211952755320125300, 1967470772556389886966582369792]


# Nine circles
#rs = [20462058208488231647417433617297079848497688661503, 20390648102633002204821975145516458134819654284000, 16319176351577787217946622420863246122794671095122, 13887086698218333155511665417755919406708188493240, 11807206420690337974351859817821893120751013059785, 10476904624282366509117433310688782603093608447872, 9219842811538324436603762533161705225844031015625, 9005821612317947755764035356994985441080520322872, 7060389660562488245299562445831405613607693415625, 4732390216546287807778695885076197588586977922872]
#rs = [3213325759500751610701783779085286711241322596321, 3156183588675354988317848135584338175793003144175, 2596506855062278338375719951091228798233856789504, 2154443718931235579319072221089531419933264225631, 1875585625550135255143813037240110308023986923200, 1776874059014646474587297648791683277415576624304, 1575714829348078360419612988032243232915568779375, 1102674442551975115316659204649879487112269794304, 999011643532822273622867560578175358581885351875, 865932317155291542105400372012194365327407989504]
#rs = [11906497765354099291316634909376637532216282501, 11443874342197805282655882696144103043029256232, 9815095676791984517754813558048101147895085992, 7838153402561546391105087274484056976318821974, 7827111852628620699676777644124145508514127400, 7081817486407631084154476418031542151268255187, 6238571798767751208271930758496003638393490600, 6154290158641520959724766439501839136494466899, 3899102781285406825668491902003836922766510592, 1487446232910753907974969992665482140686114899]
#rs = [709443224772375076490888082992428557, 692139731485243977064281056577979080, 576812057280425407234749658443028488, 472962149848250050993925388661619038, 416339920641065185734089250582411125, 390230560433723432214048960250349568, 331262009281805412536278944896689875, 270024984401713977801309216292802848, 199849675292201781541414225377080648, 199212758484241698555034320365658795]

# Normalize the radii
nrs = [r / rs[0] for r in rs]

def theta(a, b):
  n = (a+1)**2 + (b+1)**2 - (a+b)**2
  d = 2*(a+1)*(b+1)
  return math.acos(n / d)

def center_of_circle(i):
  if (i == 0):
    return (0, 0)
  t = 0.0
  for i in range(2, i+1):
    t += theta(nrs[i-1], nrs[i])
  r = nrs[0] + nrs[i]
  return (r * math.cos(t), r * -math.sin(t))

def drawcircle(i):
  cx, cy = center_of_circle(i)
  fontsize = 3*nrs[i] / len(str(rs[i]))
  print('  <circle fill="none" stroke-width="0.01" stroke="#000" cx="%g" cy="%g" r="%g"/>' % (cx, cy, nrs[i]))
  print('  <text x="%g" y="%g" text-anchor="middle" text-align="center" font-size="%f">%d</text>' % (cx, cy, fontsize, rs[i]))

def drawline(i, j):
  cxi, cyi = center_of_circle(i)
  cxj, cyj = center_of_circle(j)
  dist = nrs[i] + nrs[j]
  fontsize = dist / len(str(rs[i]))
  print('  <line stroke-width="0.01" stroke="#444" x1="%g" y1="%g" x2="%g" y2="%g"/>' % (cxi, cyi, cxj, cyj))
  print('  <text x="%g" y="%g" text-anchor="middle" text-align="center" font-size="%f">%d</text>' % ((cxi + cxj) / 2, (cyi + cyj) / 2 - fontsize, fontsize, rs[i] + rs[j]))

print('<svg xmlns="http://www.w3.org/2000/svg" width="100%" viewBox="-3 -3 6 6" style="background-color: white">')
if True:
  for i in range(len(nrs)):
    drawcircle(i)
if False:
  for i in range(1, len(nrs)):
    drawline(0, i)
    drawline(i, 1 if i == len(nrs)-1 else i+1)
print('</svg>')
